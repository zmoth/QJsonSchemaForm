#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~MainWindow() override = default;

  private:
    QJsonObject _schema{};

    QJsonDocument _doc{};
};
