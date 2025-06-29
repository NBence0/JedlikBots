import sys
import math
import os
import json
import uuid
from PyQt6.QtWidgets import (QApplication, QMainWindow, QVBoxLayout, QHBoxLayout,
                             QWidget, QPushButton, QLabel, QStatusBar, QGraphicsView,
                             QGraphicsScene, QGraphicsPixmapItem, QGraphicsLineItem,
                             QGraphicsTextItem, QButtonGroup, QFileDialog, QMessageBox)
from PyQt6.QtCore import Qt, QPointF, QRectF
from PyQt6.QtGui import QPixmap, QPen, QColor, QFont, QPainter, QImage, QAction
from PIL import Image, ImageQt

# Nagyobb képek kezelésének engedélyezése
Image.MAX_IMAGE_PIXELS = None

# --- Mérés Adatstruktúra Osztály ---
class Measurement:
    def __init__(self, type, color, points, text_info, measurement_id=None):
        self.id = measurement_id if measurement_id else str(uuid.uuid4())
        self.type = type
        self.color = color
        self.points = points
        self.text_info = text_info

    def to_dict(self):
        return {
            'id': self.id,
            'type': self.type,
            'color': QColor(self.color).name(),
            'points': self.points,
            'text_info': self.text_info,
        }

    @staticmethod
    def from_dict(data):
        return Measurement(
            type=data['type'],
            color=QColor(data['color']),
            points=data['points'],
            text_info=data['text_info'],
            measurement_id=data['id']
        )

class MeasuringGraphicsView(QGraphicsView):
    # Konstansok
    BASE_FONT_SIZE = 25
    BASE_LINE_WIDTH = 4
    BASE_CROSSHAIR_SIZE = 20
    BASE_TEXT_OFFSET = 12
    
    WALL_COLOR = QColor(255, 255, 0)
    PTP_COLOR = QColor(255, 0, 0)
    PTP45_COLOR = QColor(0, 255, 0)
    SHADOW_COLOR = QColor(0, 0, 0, 150)

    def __init__(self, status_callback):
        super().__init__()
        self.status_callback = status_callback
        
        self.setRenderHint(QPainter.RenderHint.Antialiasing)
        self.setTransformationAnchor(QGraphicsView.ViewportAnchor.AnchorUnderMouse)
        self.setResizeAnchor(QGraphicsView.ViewportAnchor.AnchorUnderMouse)
        
        self.measuring_mode = 'pan'
        self.first_point = None
        self.px_to_mm = 0.085 
        
        self.min_zoom = 0.05
        self.max_zoom = 20.0
        
        self.measurements = {} 
        self.set_mode('pan') # Kezdő mód beállítása

    def get_current_scale(self):
        return self.transform().m11()

    def set_mode(self, mode):
        self.measuring_mode = mode
        self.first_point = None
        self.clear_temp_graphics()

        # A DragMode (görgetés) és a kurzor beállítása a módnak megfelelően
        if mode == 'pan':
            self.setDragMode(QGraphicsView.DragMode.ScrollHandDrag)
            self.setCursor(Qt.CursorShape.OpenHandCursor)
        else:
            self.setDragMode(QGraphicsView.DragMode.NoDrag)
            if mode == 'delete':
                self.setCursor(Qt.CursorShape.CrossCursor) # Törléshez is célkereszt
            else:
                self.setCursor(Qt.CursorShape.CrossCursor) # Méréshez is
            
    def wheelEvent(self, event):
        if self.scene() is None or not self.scene().items():
            return
            
        zoom_in_factor = 1.15
        zoom_out_factor = 1 / zoom_in_factor
        
        current_scale = self.get_current_scale()
        
        if event.angleDelta().y() > 0:
            if current_scale * zoom_in_factor > self.max_zoom: return
            zoom_factor = zoom_in_factor
        else:
            if current_scale * zoom_out_factor < self.min_zoom: return
            zoom_factor = zoom_out_factor
            
        self.scale(zoom_factor, zoom_factor)
        self.redraw_all_measurements()
        self.status_callback()
    
    def mousePressEvent(self, event):
        # 1. Törlési mód kezelése bal kattintással
        if self.measuring_mode == 'delete' and event.button() == Qt.MouseButton.LeftButton:
            self.handle_delete_click(event.pos())
            return

        # 2. Mérési módok kezelése bal kattintással
        is_measurement_click = (event.button() == Qt.MouseButton.LeftButton and
                                self.measuring_mode not in ['pan', 'delete'])
        if is_measurement_click:
            scene_pos = self.mapToScene(event.pos())
            if self.measuring_mode == 'wall_to_wall': self.measure_wall_to_wall(scene_pos)
            elif self.measuring_mode == 'point_to_point': self.measure_point_to_point(scene_pos)
            elif self.measuring_mode == 'point_to_point_45': self.measure_point_to_point_45(scene_pos)
            return

        # 3. Törlés jobb kattintással (mint gyorsbillentyű, minden módban)
        if event.button() == Qt.MouseButton.RightButton:
            self.handle_delete_click(event.pos())
            return

        # 4. Ha egyik sem, akkor a szülő osztály kezeli (pl. a Pan mód)
        super().mousePressEvent(event)
    
    def handle_delete_click(self, view_pos):
        """Keres egy mérést a kattintás helyén és törli azt."""
        item = self.itemAt(view_pos)
        # Ellenőrizzük, hogy van-e elem, van-e hozzárendelt adat (ID), és hogy az nem "temp"
        if item and item.data(0) and item.data(0) != "temp":
            measurement_id = item.data(0)
            self.remove_measurement_by_id(measurement_id)
            self.status_callback("Mérés törölve.")

    def mouseReleaseEvent(self, event):
        super().mouseReleaseEvent(event)
        self.status_callback()
    
    def measure_point_to_point(self, point):
        if self.first_point is None:
            self.first_point = point
            self.draw_temp_crosshair(point, self.PTP_COLOR)
        else:
            dx = point.x() - self.first_point.x()
            dy = point.y() - self.first_point.y()
            distance_px = math.sqrt(dx*dx + dy*dy)
            distance_mm = int(distance_px * self.px_to_mm)
            angle_deg = int(math.degrees(math.atan2(dx, -dy)))
            
            scale = self.get_current_scale()
            offset = self.BASE_TEXT_OFFSET / scale
            mid_x = (self.first_point.x() + point.x()) / 2
            mid_y = (self.first_point.y() + point.y()) / 2
            
            measurement = Measurement(
                type='point_to_point',
                color=self.PTP_COLOR,
                points=[[self.first_point.x(), self.first_point.y()], [point.x(), point.y()]],
                text_info={'text': f"{distance_mm}mm\n{angle_deg}°", 'pos': [mid_x + offset, mid_y]}
            )
            self.add_measurement(measurement)
            self.first_point = None
            self.clear_temp_graphics()

    def measure_point_to_point_45(self, point):
        if self.first_point is None:
            self.first_point = point
            self.draw_temp_crosshair(point, self.PTP45_COLOR)
        else:
            dx = point.x() - self.first_point.x()
            dy = point.y() - self.first_point.y()
            angle_rad = math.atan2(dx, -dy)
            angle_deg = math.degrees(angle_rad)
            snapped_angle_deg = round(angle_deg / 45) * 45
            snapped_angle_rad = math.radians(snapped_angle_deg)
            distance = math.sqrt(dx*dx + dy*dy)
            new_dx = distance * math.sin(snapped_angle_rad)
            new_dy = -distance * math.cos(snapped_angle_rad)
            snapped_point = QPointF(self.first_point.x() + new_dx, self.first_point.y() + new_dy)
            distance_mm = int(distance * self.px_to_mm)

            scale = self.get_current_scale()
            offset = self.BASE_TEXT_OFFSET / scale
            mid_x = (self.first_point.x() + snapped_point.x()) / 2
            mid_y = (self.first_point.y() + snapped_point.y()) / 2

            measurement = Measurement(
                type='point_to_point_45',
                color=self.PTP45_COLOR,
                points=[[self.first_point.x(), self.first_point.y()], [snapped_point.x(), snapped_point.y()]],
                text_info={'text': f"{distance_mm}mm\n{snapped_angle_deg}°", 'pos': [mid_x + offset, mid_y]}
            )
            self.add_measurement(measurement)
            self.first_point = None
            self.clear_temp_graphics()

    def measure_wall_to_wall(self, point):
        scene_rect = self.scene().itemsBoundingRect()
        
        to_top_mm = int((point.y() - scene_rect.top()) * self.px_to_mm)
        to_bottom_mm = int((scene_rect.bottom() - point.y()) * self.px_to_mm)
        to_left_mm = int((point.x() - scene_rect.left()) * self.px_to_mm)
        to_right_mm = int((scene_rect.right() - point.x()) * self.px_to_mm)
        
        scale = self.get_current_scale()
        offset = self.BASE_TEXT_OFFSET / scale

        meas_top = Measurement(
            'wall_to_wall', self.WALL_COLOR, [[point.x(), point.y()], [point.x(), scene_rect.top()]],
            {'text': f"↑ {to_top_mm}mm", 'pos': [point.x() + offset, (point.y() + scene_rect.top()) / 2]})
        meas_bottom = Measurement(
            'wall_to_wall', self.WALL_COLOR, [[point.x(), point.y()], [point.x(), scene_rect.bottom()]],
            {'text': f"↓ {to_bottom_mm}mm", 'pos': [point.x() + offset, (point.y() + scene_rect.bottom()) / 2]})
        meas_left = Measurement(
            'wall_to_wall', self.WALL_COLOR, [[point.x(), point.y()], [scene_rect.left(), point.y()]],
            {'text': f"← {to_left_mm}mm", 'pos': [(point.x() + scene_rect.left()) / 2, point.y() - offset]})
        meas_right = Measurement(
            'wall_to_wall', self.WALL_COLOR, [[point.x(), point.y()], [scene_rect.right(), point.y()]],
            {'text': f"→ {to_right_mm}mm", 'pos': [(point.x() + scene_rect.right()) / 2, point.y() - offset]})

        for m in [meas_top, meas_bottom, meas_left, meas_right]:
            self.add_measurement(m)

    def add_measurement(self, measurement):
        self.measurements[measurement.id] = measurement
        self.draw_measurement(measurement)

    def draw_measurement(self, measurement):
        pen = self.get_dynamic_pen(measurement.color)
        p1 = measurement.points[0]
        p2 = measurement.points[1]
        line = self.scene().addLine(p1[0], p1[1], p2[0], p2[1], pen)
        line.setData(0, measurement.id)

        text_info = measurement.text_info
        self.draw_measurement_text(text_info['pos'][0], text_info['pos'][1],
                                   text_info['text'], measurement.color, measurement.id)

    def draw_measurement_text(self, x, y, text, color, measurement_id):
        scale = self.get_current_scale()
        font_size = max(1, int(self.BASE_FONT_SIZE / scale))
        font = QFont("Arial", font_size)
        font.setBold(True)

        shadow_offset = max(0.2, (self.BASE_LINE_WIDTH / 3) / scale)

        shadow_item = QGraphicsTextItem(text)
        shadow_item.setFont(font)
        shadow_item.setDefaultTextColor(self.SHADOW_COLOR)
        shadow_item.setPos(x + shadow_offset, y + shadow_offset)
        shadow_item.setZValue(1)
        
        text_item = QGraphicsTextItem(text)
        text_item.setFont(font)
        text_item.setDefaultTextColor(color)
        text_item.setPos(x, y)
        text_item.setZValue(2)
        
        shadow_item.setData(0, measurement_id)
        text_item.setData(0, measurement_id)
        
        self.scene().addItem(shadow_item)
        self.scene().addItem(text_item)
        
    def get_dynamic_pen(self, color):
        scale = self.get_current_scale()
        pen_width = max(0.5, self.BASE_LINE_WIDTH / scale)
        return QPen(color, pen_width)

    def draw_temp_crosshair(self, point, color):
        pen = self.get_dynamic_pen(color)
        scale = self.get_current_scale()
        size = self.BASE_CROSSHAIR_SIZE / scale
        
        line1 = self.scene().addLine(point.x() - size, point.y(), point.x() + size, point.y(), pen)
        line2 = self.scene().addLine(point.x(), point.y() - size, point.x(), point.y() + size, pen)
        line1.setData(0, "temp")
        line2.setData(0, "temp")

    def clear_temp_graphics(self):
        """Törli az ideiglenes grafikákat (pl. első kattintás keresztje)."""
        # VÉDELEM: Csak akkor fusson, ha már van scene beállítva. Ez javítja az indítási hibát.
        if not self.scene():
            return
            
        items_to_remove = [item for item in self.scene().items() if item.data(0) == "temp"]
        for item in items_to_remove:
            self.scene().removeItem(item)
    
    def remove_measurement_by_id(self, measurement_id):
        if measurement_id in self.measurements:
            del self.measurements[measurement_id]
        
        items_to_remove = [item for item in self.scene().items() if item.data(0) == measurement_id]
        for item in items_to_remove:
            self.scene().removeItem(item)

    def _clear_measurement_graphics(self):
        items_to_remove = [item for item in self.scene().items() 
                           if isinstance(item, (QGraphicsLineItem, QGraphicsTextItem))]
        for item in items_to_remove:
            self.scene().removeItem(item)

    def redraw_all_measurements(self):
        self._clear_measurement_graphics()
        for measurement in self.measurements.values():
            self.draw_measurement(measurement)

    def clear_all_measurements(self):
        self._clear_measurement_graphics()
        self.measurements.clear()
        self.first_point = None

class ImageMeasuringApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Kép Mérő Alkalmazás v6.0 (Egyenkénti törlés)")
        self.setGeometry(100, 100, 1500, 900)
        
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        layout = QVBoxLayout(central_widget)
        
        self.create_menu()
        
        button_layout = QHBoxLayout()
        # Gombok létrehozása
        self.pan_button = QPushButton("Mozgatás")
        self.pan_button.setCheckable(True)
        self.wall_button = QPushButton("Faltól-falig")
        self.wall_button.setCheckable(True)
        self.point_button = QPushButton("Pont-pont")
        self.point_button.setCheckable(True)
        self.point45_button = QPushButton("Pont-pont 45°")
        self.point45_button.setCheckable(True)
        self.delete_button = QPushButton("Mérés törlése")
        self.delete_button.setCheckable(True)
        self.clear_all_button = QPushButton("Összes mérés törlése")
        
        # Gombcsoport a "rádiógomb" viselkedéshez
        self.button_group = QButtonGroup(self)
        self.button_group.setExclusive(True)
        self.button_group.addButton(self.pan_button)
        self.button_group.addButton(self.wall_button)
        self.button_group.addButton(self.point_button)
        self.button_group.addButton(self.point45_button)
        self.button_group.addButton(self.delete_button)
        
        # Eseménykezelők
        self.pan_button.clicked.connect(lambda: self.set_measuring_mode('pan'))
        self.wall_button.clicked.connect(lambda: self.set_measuring_mode('wall_to_wall'))
        self.point_button.clicked.connect(lambda: self.set_measuring_mode('point_to_point'))
        self.point45_button.clicked.connect(lambda: self.set_measuring_mode('point_to_point_45'))
        self.delete_button.clicked.connect(lambda: self.set_measuring_mode('delete'))
        self.clear_all_button.clicked.connect(self.clear_all_measurements)

        # Gombok elhelyezése a layoutban
        button_layout.addWidget(self.pan_button)
        button_layout.addWidget(self.wall_button)
        button_layout.addWidget(self.point_button)
        button_layout.addWidget(self.point45_button)
        button_layout.addSpacing(20)
        button_layout.addWidget(self.delete_button)
        button_layout.addWidget(self.clear_all_button)
        button_layout.addStretch()
        
        self.graphics_view = MeasuringGraphicsView(self.update_status)
        self.graphics_scene = QGraphicsScene(self)
        self.graphics_view.setScene(self.graphics_scene)
        
        layout.addLayout(button_layout)
        layout.addWidget(self.graphics_view)
        
        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        self.zoom_label = QLabel("Zoom: -")
        self.status_bar.addPermanentWidget(self.zoom_label)
        
        self.image_item = None
        self.update_button_states()

    def create_menu(self):
        menu_bar = self.menuBar()
        file_menu = menu_bar.addMenu("&Fájl")

        self.load_action = QAction("&Kép betöltése...", self)
        self.load_action.triggered.connect(self.load_image)
        file_menu.addAction(self.load_action)

        self.save_action = QAction("&Mentés képként...", self)
        self.save_action.triggered.connect(self.export_image_with_measurements)
        file_menu.addAction(self.save_action)

        file_menu.addSeparator()
        
        self.import_action = QAction("&Mérések importálása (JSON)...", self)
        self.import_action.triggered.connect(self.import_measurements)
        file_menu.addAction(self.import_action)

        self.export_action = QAction("&Mérések exportálása (JSON)...", self)
        self.export_action.triggered.connect(self.export_measurements)
        file_menu.addAction(self.export_action)
        
        file_menu.addSeparator()

        self.exit_action = QAction("&Kilépés", self)
        self.exit_action.triggered.connect(self.close)
        file_menu.addAction(self.exit_action)

    def load_image(self):
        file_path, _ = QFileDialog.getOpenFileName(self, "Kép kiválasztása", "", "Képfájlok (*.jpg *.jpeg *.png *.bmp)")
        if not file_path: return

        try:
            self.graphics_scene.clear()
            self.graphics_view.measurements.clear()
            
            pil_image = Image.open(file_path).convert('RGBA')
            pixmap = QPixmap.fromImage(ImageQt.ImageQt(pil_image))
            
            self.image_item = self.graphics_scene.addPixmap(pixmap)
            self.image_item.setZValue(0)
            
            self.graphics_scene.setSceneRect(QRectF(pixmap.rect()))
            self.graphics_view.fitInView(self.image_item, Qt.AspectRatioMode.KeepAspectRatio)
            
            self.update_button_states()
            self.set_default_mode()
            self.update_status(f"Kép betöltve: {os.path.basename(file_path)}")
        except Exception as e:
            QMessageBox.critical(self, "Hiba", f"Kép betöltési hiba:\n{e}")
            self.image_item = None
            self.update_button_states()

    def export_image_with_measurements(self):
        if not self.image_item: return
        file_path, _ = QFileDialog.getSaveFileName(self, "Kép mentése", "meresekkel.png", "PNG Kép (*.png);;JPEG Kép (*.jpg)")
        if not file_path: return
        try:
            source_rect = self.graphics_scene.itemsBoundingRect()
            image = QImage(source_rect.size().toSize(), QImage.Format.Format_ARGB32_Premultiplied)
            image.fill(Qt.GlobalColor.transparent)

            painter = QPainter(image)
            painter.setRenderHint(QPainter.RenderHint.Antialiasing)
            
            self.graphics_scene.render(painter, QRectF(image.rect()), source_rect)
            painter.end()
            
            image.save(file_path)
            self.update_status(f"Kép sikeresen mentve: {file_path}")
        except Exception as e:
            QMessageBox.critical(self, "Hiba", f"Kép mentési hiba:\n{e}")

    def export_measurements(self):
        if not self.graphics_view.measurements:
            QMessageBox.information(self, "Info", "Nincsenek mérések, amiket exportálni lehetne.")
            return

        file_path, _ = QFileDialog.getSaveFileName(self, "Mérések exportálása", "meresek.json", "JSON Fájlok (*.json)")
        if not file_path: return

        try:
            data_to_export = [m.to_dict() for m in self.graphics_view.measurements.values()]
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(data_to_export, f, indent=4)
            self.update_status("Mérések sikeresen exportálva.")
        except Exception as e:
            QMessageBox.critical(self, "Hiba", f"Exportálási hiba:\n{e}")

    def import_measurements(self):
        if not self.image_item:
            QMessageBox.warning(self, "Hiba", "Kérlek, először tölts be egy képet a mérések importálásához.")
            return

        file_path, _ = QFileDialog.getOpenFileName(self, "Mérések importálása", "", "JSON Fájlok (*.json)")
        if not file_path: return
        
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                imported_data = json.load(f)
            
            self.graphics_view.clear_all_measurements() # A megerősítő dialógus nélküli verzió

            for data in imported_data:
                measurement = Measurement.from_dict(data)
                self.graphics_view.add_measurement(measurement)
            
            self.graphics_view.redraw_all_measurements()
            self.update_status(f"{len(imported_data)} mérés sikeresen importálva.")
        except Exception as e:
            QMessageBox.critical(self, "Hiba", f"Importálási hiba:\n{e}")

    def set_measuring_mode(self, mode):
        self.graphics_view.set_mode(mode)
    
    def set_default_mode(self):
        self.pan_button.setChecked(True)
        self.set_measuring_mode('pan')

    def clear_all_measurements(self):
        if not self.graphics_view.measurements: return
        
        reply = QMessageBox.question(self, 'Megerősítés', 
                                     'Biztosan törölni szeretnéd az ÖSSZES mérést?',
                                     QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No, 
                                     QMessageBox.StandardButton.No)
        if reply == QMessageBox.StandardButton.Yes:
            self.graphics_view.clear_all_measurements()
            self.update_status("Minden mérés törölve.")
    
    def update_button_states(self):
        is_image_loaded = self.image_item is not None
        
        # Gombok állapotának frissítése
        for btn in [self.pan_button, self.wall_button, self.point_button, 
                    self.point45_button, self.delete_button, self.clear_all_button]:
            btn.setEnabled(is_image_loaded)
        
        # Menüpontok állapotának frissítése
        self.save_action.setEnabled(is_image_loaded)
        self.import_action.setEnabled(is_image_loaded)
        self.export_action.setEnabled(is_image_loaded)
    
    def update_status(self, message=None):
        if message:
            self.status_bar.showMessage(message, 3000)
        if self.image_item:
            zoom_percent = int(self.graphics_view.get_current_scale() * 100)
            self.zoom_label.setText(f"Zoom: {zoom_percent}%")
        else:
            self.zoom_label.setText("Zoom: -")

def main():
    app = QApplication(sys.argv)
    window = ImageMeasuringApp()
    window.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()