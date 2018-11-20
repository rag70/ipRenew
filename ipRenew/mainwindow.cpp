#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QSettings>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  m_delay(100*60),
  timer(nullptr)
{
  ui->setupUi(this);

  QSystemTrayIcon *trayIcon = new QSystemTrayIcon(this);
  trayIcon->setIcon(QIcon(":/gui/appLogo.png"));

  connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));

  QString ls("ms;second;minute;hours;days");
  ui->cbUnit->addItems(ls.split(";"));
  ui->cbUnit->setCurrentIndex(2);

  ui->txDelay->setValue(30);

  trayIconMenu = new QMenu(this);
  trayIconMenu->addAction(ui->actionUpdate);
  trayIconMenu->addAction(ui->actionMinimize);
  trayIconMenu->addAction(ui->actionRestore);
  trayIconMenu->addAction(ui->actionExit);

  trayIcon->setContextMenu (trayIconMenu);

  loadSetting();

  update();

  trayIcon->show();

  startTimer();
}

MainWindow::~MainWindow()
{
  saveSetting();
  delete ui;
}

void MainWindow::startTimer()
{
  if (timer==nullptr)
  {
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
   }
  else
  {
      timer->stop();
  }
  timer->start(m_delay);
}

void MainWindow::trayClicked(QSystemTrayIcon::ActivationReason e)
{
    if( e == QSystemTrayIcon::Trigger)
     {
        if(this->isVisible() == true)
          this->hide();
        else
          this->show();
    }
}

void MainWindow::startRequest(QUrl url)
{

    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void MainWindow::on_actionRestore_triggered()
{
    this->show();
}

void MainWindow::on_actionMinimize_triggered()
{
  this->hide();
}

void MainWindow::on_actionExit_triggered()
{
  this->close();
}

void MainWindow::changeTimer()
{
  // "ms;second;minute;hours;days"
  int delay = ui->txDelay->value();
  int unit = ui->cbUnit->currentIndex();
  m_delay = 1000;
  switch (unit)
  {
    case 0: // ms
      m_delay=delay;
      break;
    case 1: //s
      m_delay*=delay;
      break;
    case 2: // minute
      m_delay*=delay*60;
      break;
    case 3: // hour
      m_delay*=delay*60*60;
      break;
    case 4: // days
      m_delay*=delay*60*60*24;
      break;
    }
  if (m_delay<500)
    m_delay = 500;
  startTimer();
}

void MainWindow::update()
{
  QUrl url( ui->urlEdit->text());
  startRequest(url);
}

void MainWindow::httpReadyRead()
{
  qint64 size = reply->bytesAvailable();
  QString test = reply->read(size);
  ui->lbResult->setText(test);
  QDateTime local(QDateTime::currentDateTime());
  ui->lbLastUpdate->setText("Last Update: "+local.toString());
}

void MainWindow::on_actionUpdate_triggered()
{
    update();
}


void MainWindow::on_actionReload_triggered()
{
    // reload all the configuration

}

void MainWindow::loadSetting()
{
  QSettings settings("lynza", "ipRenew");
  settings.beginGroup("data");
  if (settings.contains("position"))
  {
    setGeometry(settings.value("position").toRect());
    ui->urlEdit->setText(settings.value("url").toString());
    ui->txDelay->setValue( settings.value("delay").toInt());
    ui->cbUnit->setCurrentIndex(settings.value("unit").toInt());
    int ck=settings.value("startup").toInt();
    ui->ckStartUp->setChecked(ck);
  }
  settings.endGroup();
  on_actionReload_triggered();
}

void MainWindow::saveSetting()
{
  QSettings settings("lynza", "ipRenew");
  settings.beginGroup("data");
  settings.setValue("position",this->geometry());
  settings.setValue("url",ui->urlEdit->text());
  settings.setValue("delay",ui->txDelay->text());
  settings.setValue("unit",ui->cbUnit->currentIndex());
  settings.setValue("startup",ui->ckStartUp->checkState());
  settings.endGroup();
}

void MainWindow::startupToggled(bool checked)
{
  QSettings settings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
  if (checked)
  {
         QString value = QCoreApplication::applicationFilePath();
         value.replace("/","\\");
         settings.setValue("ipRenew", value);
  }
  else
     settings.remove("ipRenew");
}


void MainWindow::on_actionOpenFreeDns_triggered()
{

  QDesktopServices::openUrl(QUrl("http://freedns.afraid.org/", QUrl::TolerantMode));
}

