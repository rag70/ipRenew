#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QtGui>
#include <iostream>
#include <QTimer>
#include <QtNetwork>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void startTimer();
  void startRequest(QUrl url);
  void download(QString url);
  void loadSetting();
  void saveSetting();


private:
  Ui::MainWindow *ui;
  int  m_delay;
  QMenu *trayIconMenu;
  QTimer *timer;
  QUrl url;
  QNetworkAccessManager qnam;
  QNetworkReply *reply;

private slots:
    void trayClicked(QSystemTrayIcon::ActivationReason e);
    void on_actionRestore_triggered();
    void on_actionMinimize_triggered();
    void on_actionExit_triggered();
    void changeTimer();
    void update();
    void startupToggled(bool checked);
   // network
    void httpReadyRead();

    void on_actionUpdate_triggered();
    void on_actionReload_triggered();
    void on_actionOpenFreeDns_triggered();
};

#endif
