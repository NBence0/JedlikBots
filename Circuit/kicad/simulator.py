import sys
import numpy as np
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QLineEdit, QPushButton, QGridLayout, QComboBox
)
from PyQt6.QtGui import QDoubleValidator, QFont
from PyQt6.QtCore import Qt

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

# --- Számítási Motor ---
class CalculationEngine:
    """
    Ez az osztály végzi az összes fizikai és elektromos számítást.
    """
    COPPER_RESISTIVITY = 1.68e-8  # Réz fajlagos ellenállása (Ohm * m) @ 20°C

    def __init__(self, length_mm, width_mm, thickness_um, current_a, layer_type='külső'):
        # Bemeneti adatok validálása és tárolása méterben
        try:
            self.length_m = float(length_mm) / 1000.0
            self.width_m = float(width_mm) / 1000.0
            self.thickness_m = float(thickness_um) / 1_000_000.0
            self.current_a = float(current_a)
            self.layer_type = layer_type
        except (ValueError, TypeError):
            raise ValueError("A bemeneti adatoknak számoknak kell lenniük.")

        if self.width_m <= 0 or self.thickness_m <= 0:
            raise ValueError("A szélességnek és vastagságnak pozitívnak kell lennie.")
            
        self.cross_section_m2 = self.width_m * self.thickness_m

    def calculate_cross_section_mm2(self):
        """Keresztmetszet számítása mm²-ben."""
        return self.cross_section_m2 * 1_000_000.0

    def calculate_resistance_ohm(self):
        """Ellenállás számítása Ohm-ban."""
        if self.cross_section_m2 == 0:
            return float('inf')
        return self.COPPER_RESISTIVITY * (self.length_m / self.cross_section_m2)

    def calculate_voltage_drop_v(self):
        """Feszültségesés számítása Volt-ban."""
        return self.calculate_resistance_ohm() * self.current_a

    def calculate_power_dissipation_w(self):
        """Hőtermelés (disszipált teljesítmény) számítása Watt-ban."""
        return self.calculate_voltage_drop_v() * self.current_a
        
    def calculate_current_density_A_mm2(self):
        """Áramsűrűség számítása A/mm²-ben."""
        if self.calculate_cross_section_mm2() == 0:
            return float('inf')
        return self.current_a / self.calculate_cross_section_mm2()

    def estimate_temperature_rise_C(self):
        """
        Hőmérséklet-emelkedés becslése az IPC-2221A szabvány képlete alapján.
        A képlet: I = k * ΔT^b * A^c, ebből ΔT-t kifejezve.
        """
        # Átváltás mil-re, mert az IPC képlet azt használja
        # 1 mm = 39.3701 mil
        # 1 m^2 = (39370.1)^2 mil^2
        cross_section_mil2 = self.cross_section_m2 * (39370.1 ** 2)

        if cross_section_mil2 <= 0:
            return 0

        # IPC-2221A konstansok
        if self.layer_type == 'belső':
            k, b, c = 0.024, 0.44, 0.725
        else:  # Külső réteg az alapértelmezett
            k, b, c = 0.048, 0.44, 0.725
            
        # A képlet átrendezve ΔT-re: ΔT = (I / (k * A^c))^(1/b)
        if self.current_a == 0:
            return 0

        try:
            temp_rise = (self.current_a / (k * (cross_section_mil2 ** c))) ** (1 / b)
        except (OverflowError, ZeroDivisionError):
            temp_rise = float('inf') # Túl nagy áram vagy túl kicsi keresztmetszet
            
        return temp_rise

# --- Matplotlib Grafikon Widget ---
class MplCanvas(FigureCanvas):
    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)
        super(MplCanvas, self).__init__(fig)

# --- Főablak ---
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Python PCB Vezetősáv Kalkulátor")
        self.setGeometry(100, 100, 900, 600)

        # Globális validátor a szám beviteli mezőkhöz
        self.validator = QDoubleValidator(0, 100000.0, 4)
        self.validator.setNotation(QDoubleValidator.Notation.StandardNotation)

        # Központi widget és layout
        main_widget = QWidget(self)
        self.setCentralWidget(main_widget)
        main_layout = QHBoxLayout(main_widget)

        # Bal oldali panel (beviteli adatok és eredmények)
        left_panel = QWidget()
        left_layout = QVBoxLayout(left_panel)
        main_layout.addWidget(left_panel, 1)

        # Jobb oldali panel (grafikon)
        right_panel = QWidget()
        right_layout = QVBoxLayout(right_panel)
        main_layout.addWidget(right_panel, 2)

        # --- Beviteli mezők ---
        input_grid = QGridLayout()
        
        # Címkék és beviteli mezők létrehozása
        self.inputs = {}
        input_data = [
            ("Vezetősáv hossza (mm)", "100"),
            ("Vezetősáv szélessége (mm)", "0.8"),
            ("Áram (A)", "1"),
            ("Tápfeszültség (V)", "5")
        ]

        for i, (label_text, default_value) in enumerate(input_data):
            label = QLabel(label_text)
            line_edit = QLineEdit(default_value)
            line_edit.setValidator(self.validator)
            input_grid.addWidget(label, i, 0)
            input_grid.addWidget(line_edit, i, 1)
            self.inputs[label_text] = line_edit

        # Réz vastagság speciális kezelése (választóval)
        self.copper_thickness_label = QLabel("Réz vastagság")
        self.copper_thickness_input = QLineEdit("35")
        self.copper_thickness_input.setValidator(self.validator)
        self.copper_unit_combo = QComboBox()
        self.copper_unit_combo.addItems(["µm", "oz"])
        self.copper_unit_combo.currentIndexChanged.connect(self.update_thickness_placeholder)
        
        thickness_layout = QHBoxLayout()
        thickness_layout.addWidget(self.copper_thickness_input)
        thickness_layout.addWidget(self.copper_unit_combo)
        input_grid.addWidget(self.copper_thickness_label, len(input_data), 0)
        input_grid.addLayout(thickness_layout, len(input_data), 1)

        # Réteg típusa
        self.layer_type_label = QLabel("Réteg típusa")
        self.layer_type_combo = QComboBox()
        self.layer_type_combo.addItems(["külső", "belső"])
        input_grid.addWidget(self.layer_type_label, len(input_data) + 1, 0)
        input_grid.addWidget(self.layer_type_combo, len(input_data) + 1, 1)
        
        # Számítás gomb
        self.calc_button = QPushButton("Számítás")
        self.calc_button.clicked.connect(self.perform_calculation)

        left_layout.addLayout(input_grid)
        left_layout.addWidget(self.calc_button)

        # --- Eredmények ---
        results_grid = QGridLayout()
        results_grid.setContentsMargins(0, 20, 0, 0)
        
        self.results = {}
        result_labels = [
            "Keresztmetszet (mm²)", "Ellenállás (mOhm)", "Feszültségesés (mV)",
            "Hőtermelés (mW)", "Áram sűrűség (A/mm²)", "Hőemelkedés becslés (°C)"
        ]

        title_font = QFont()
        title_font.setBold(True)
        results_title = QLabel("Számított adatok")
        results_title.setFont(title_font)
        left_layout.addWidget(results_title)
        
        for i, label_text in enumerate(result_labels):
            label = QLabel(f"{label_text}:")
            value_label = QLabel("-")
            results_grid.addWidget(label, i, 0)
            results_grid.addWidget(value_label, i, 1, alignment=Qt.AlignmentFlag.AlignRight)
            self.results[label_text] = value_label

        left_layout.addLayout(results_grid)
        left_layout.addStretch() # Helykitöltő

        # --- Grafikon ---
        graph_title = QLabel("Hőmérséklet-emelkedés vs. Áram")
        graph_title.setFont(title_font)
        self.canvas = MplCanvas(self, width=5, height=4, dpi=100)
        right_layout.addWidget(graph_title)
        right_layout.addWidget(self.canvas)
        
        self.perform_calculation() # Kezdeti számítás az alapértelmezett értékekkel

    def update_thickness_placeholder(self):
        """Frissíti a vastagság beviteli mezőt, ha a mértékegység változik."""
        if self.copper_unit_combo.currentText() == "oz":
            self.copper_thickness_input.setText("1")
        else: # µm
            self.copper_thickness_input.setText("35")

    def get_thickness_in_um(self):
        """Visszaadja a réz vastagságát mindig mikrométerben."""
        value = float(self.copper_thickness_input.text().replace(',', '.'))
        if self.copper_unit_combo.currentText() == "oz":
            # 1 oz ≈ 35 µm
            return value * 35.0
        return value

    def perform_calculation(self):
        """Elvégzi a számításokat és frissíti a GUI-t."""
        try:
            # Bemeneti adatok beolvasása
            length = self.inputs["Vezetősáv hossza (mm)"].text().replace(',', '.')
            width = self.inputs["Vezetősáv szélessége (mm)"].text().replace(',', '.')
            current = self.inputs["Áram (A)"].text().replace(',', '.')
            thickness_um = self.get_thickness_in_um()
            layer_type = self.layer_type_combo.currentText()

            # Számítási motor inicializálása
            engine = CalculationEngine(length, width, thickness_um, current, layer_type)

            # Eredmények frissítése a GUI-n
            self.results["Keresztmetszet (mm²)"].setText(f"{engine.calculate_cross_section_mm2():.4f}")
            self.results["Ellenállás (mOhm)"].setText(f"{engine.calculate_resistance_ohm() * 1000:.4f}")
            self.results["Feszültségesés (mV)"].setText(f"{engine.calculate_voltage_drop_v() * 1000:.4f}")
            self.results["Hőtermelés (mW)"].setText(f"{engine.calculate_power_dissipation_w() * 1000:.4f}")
            self.results["Áram sűrűség (A/mm²)"].setText(f"{engine.calculate_current_density_A_mm2():.2f}")
            
            temp_rise = engine.estimate_temperature_rise_C()
            temp_label = self.results["Hőemelkedés becslés (°C)"]
            temp_label.setText(f"{temp_rise:.2f}")
            
            # Figyelmeztetés, ha a melegedés magas
            if temp_rise > 80:
                temp_label.setStyleSheet("color: red; font-weight: bold;")
            elif temp_rise > 40:
                temp_label.setStyleSheet("color: orange;")
            else:
                temp_label.setStyleSheet("color: black;")


            # Grafikon frissítése
            self.update_plot(engine)

        except (ValueError, ZeroDivisionError) as e:
            # Hiba esetén a mezők alaphelyzetbe állítása
            for label in self.results.values():
                label.setText("Hiba!")
                label.setStyleSheet("color: red;")
            print(f"Hiba a számítás során: {e}")
        except Exception as e:
            print(f"Váratlan hiba: {e}")

    def update_plot(self, current_engine):
        """Frissíti a Matplotlib grafikont."""
        self.canvas.axes.clear()
        
        # Áram-tartomány a grafikonhoz
        max_current = float(self.inputs["Áram (A)"].text().replace(',', '.')) * 2
        if max_current < 1: max_current = 1
        currents = np.linspace(0.01, max_current, 100)
        
        temps = []
        for i in currents:
            engine = CalculationEngine(
                current_engine.length_m * 1000,
                current_engine.width_m * 1000,
                current_engine.thickness_m * 1_000_000,
                i,
                current_engine.layer_type
            )
            temps.append(engine.estimate_temperature_rise_C())

        self.canvas.axes.plot(currents, temps, color='r')
        
        # Jelenlegi pont megjelölése
        current_temp = current_engine.estimate_temperature_rise_C()
        self.canvas.axes.plot(current_engine.current_a, current_temp, 'bo', label=f'Jelenlegi pont ({current_engine.current_a:.2f} A, {current_temp:.2f} °C)')
        
        self.canvas.axes.set_xlabel("Áram (A)")
        self.canvas.axes.set_ylabel("Hőmérséklet-emelkedés (°C)")
        self.canvas.axes.set_title("Hőemelkedés az áram függvényében")
        self.canvas.axes.grid(True)
        self.canvas.axes.legend()
        self.canvas.draw()


if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
