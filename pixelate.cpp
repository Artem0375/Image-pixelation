#include <opencv2/opencv.hpp>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFileDialog>
#include <QWheelEvent>
#include <QVBoxLayout>

using namespace cv;
using namespace std;


void pixelate(Mat& src, Mat& dst, int pixel_size) {
  // Проверяем, что размер пикселя больше нуля и меньше размера изображения
  assert(pixel_size > 0 && pixel_size < min(src.cols, src.rows));
  // Создаем копию исходного изображения
  dst = src.clone();
  // Проходим по всем пикселям изображения с шагом равным размеру пикселя
  for (int i = 0; i < src.rows; i += pixel_size) {
    for (int j = 0; j < src.cols; j += pixel_size) {
      // Вычисляем координаты прямоугольника, который будет заменен одним цветом
      int x = min(i + pixel_size, src.rows);
      int y = min(j + pixel_size, src.cols);
      Rect rect(j, i, y - j, x - i);
      // Вычисляем средний цвет прямоугольника
      Scalar color = mean(src(rect));
      // Заполняем прямоугольник средним цветом
      dst(rect) = color;
    }
  }
 
  GaussianBlur(dst, dst, Size(3, 3), 0);
}

class MainWindow : public QMainWindow {
 
  QLabel* label_src; // Метка для отображения исходного изображения
  QLabel* label_dst; // Метка для отображения пикселизованного изображения
  QSlider* slider; // Слайдер для выбора размера пикселя
  QPushButton* button; // Кнопка для выбора файла изображения
  QPushButton* saveButton; // Кнопка для сохранения изображения
  Mat src; 
  Mat dst; // Матрица для хранения пикселизованного изображения


public:
  // Конструктор класса
MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
    setWindowTitle("Pixelator");
    setMinimumSize(800, 600);
    setMaximumSize(1920, 1080);

    // Создаем элементы интерфейса
    label_src = new QLabel;
    label_src->setStyleSheet("border: 1px solid black");
    label_src->setScaledContents(true);

    label_dst = new QLabel;
    label_dst->setStyleSheet("border: 1px solid black");
    label_dst->setScaledContents(true);

    slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(1);
    slider->setMaximum(100);
    slider->setValue(10);

    button = new QPushButton("Choose image file");
    saveButton = new QPushButton("Save image", this);

   
    // Подключаем слоты к сигналам элементов интерфейса
    connect(slider, &QSlider::valueChanged, this, &MainWindow::updatePixelSize);
    connect(button, &QPushButton::clicked, this, &MainWindow::chooseImageFile);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveImage);

    // Создаем механизм размещения и добавляем в него элементы интерфейса
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(label_src);
    layout->addWidget(label_dst);
    layout->addWidget(slider);
    layout->addWidget(button);
    layout->addWidget(saveButton);
    // Создаем центральный виджет и устанавливаем его для главного окна
    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);
}


void chooseImageFile() {
  QString filename = QFileDialog::getOpenFileName(this, "Choose image file", "", "Image files (*.jpg *.png *.bmp)");
  if (!filename.isEmpty()) {
    src = imread(filename.toStdString());
    if (!src.empty()) {
      QImage image = QImage((uchar*)src.data, src.cols, src.rows, src.step, QImage::Format_RGB888).rgbSwapped();
      QPixmap pixmap = QPixmap::fromImage(image);
      int w = label_src->width();
      int h = label_src->height();
      // Масштабируем QPixmap, чтобы он соответствовал размеру QLabel
      label_src->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
      updatePixelSize(slider->value());
    }
  }
}

  // Слот для обновления размера пикселя
void updatePixelSize(int value) {
  if (!src.empty()) {
    // Сохраняем исходный размер изображения
    Size originalSize = src.size();

    // Применяем эффект пикселизации
    pixelate(src, dst, value);

    // Масштабируем изображение обратно к исходному размеру
    cv::resize(dst, dst, originalSize);

    QImage image = QImage((uchar*)dst.data, dst.cols, dst.rows, dst.step, QImage::Format_RGB888).rgbSwapped();
    QPixmap pixmap = QPixmap::fromImage(image);
    int w = label_dst->width();
    int h = label_dst->height();
    // Масштабируем QPixmap, чтобы он соответствовал размеру QLabel
    label_dst->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
  }
}

  void saveImage() {
    QString filename = QFileDialog::getSaveFileName(this, "Save image", "", "Image files (*.jpg *.png *.bmp)");
    if (!filename.isEmpty()) {
      imwrite(filename.toStdString(), dst);
    }
  }

};

int main(int argc, char* argv[]) {
  
  QApplication app(argc, argv);
  MainWindow window;
  window.show();
  // Запускаем цикл обработки событий приложения
  return app.exec();
}

