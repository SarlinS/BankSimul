#include "tapahtumawindow.h"
#include "ui_tapahtumawindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QCoreApplication>
#include <QHeaderView>
#include <QMessageBox>

tapahtumaWindow::tapahtumaWindow(QByteArray token,QString myCard,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tapahtumaWindow)
{
    ui->setupUi(this);
    webToken=token;
    card_number = myCard;
    qDebug()<<card_number;
    ui->tapahtumaTable->setRowCount(100);
    ui->tapahtumaTable->setColumnCount(6);

    ui->tapahtumaTable->verticalHeader()->setVisible(false);
    ui->tapahtumaTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tapahtumaTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tapahtumaTable->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tapahtumaTable->setShowGrid(false);
    ui->tapahtumaTable->setStyleSheet("QTableView {selection-background-color: green;}");
    //ui->tapahtumaTable->setGeometry(QApplication::desktop()->screenGeometry());
    QString site_url=url::getBaseUrl()+"cards/" + card_number;
    QNetworkRequest request((site_url));
    qDebug()<<site_url;
    //WEBTOKEN ALKU
    request.setRawHeader(QByteArray("Authorization"),(webToken));
    //WEBTOKEN LOPPU
    tapahtumaManager = new QNetworkAccessManager(this);

    connect(tapahtumaManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(tapahtumaSlot(QNetworkReply*)));
    reply = tapahtumaManager->get(request);

}

tapahtumaWindow::~tapahtumaWindow()
{
    delete ui;
}

void tapahtumaWindow::delay()
{
    int afkTimer=30; //afkTimer=30 tarkoittaa 30 sekuntia. Muokkaa lyhyemmäksi kun testailet.
    QTime dieTime= QTime::currentTime().addSecs(afkTimer);
     while (QTime::currentTime() < dieTime)
         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
     qDebug()<<"afkTimer 30sec";
     this->close(); //Tähän pitää keksiä järkevä funktio että menee aloitusnäkymään

}
void tapahtumaWindow::tapahtumaSlot(QNetworkReply *reply)
{
    QByteArray response_data=reply->readAll();
    qDebug()<<response_data; //toimii tuo tietokannasta tiedot
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonArray json_array = json_doc.array();
    QJsonObject json_obj = json_doc.object();

    QString account = QString::number(json_obj["id_account"].toInt());
    QString card_number = QString::number(json_obj["card_number"].toInt());
    QString debit_credit = json_obj["debit_credit"].toString();
    QString card_owner = json_obj["card_owner"].toString();

    lista += "Account ID: " + account + "\nCard number: "+card_number + "\nDebit/Credit: "+debit_credit +"\nCard owner: "+card_owner;

    qDebug()<<"cards data in tapahtumawindow: "<<account<<card_number<<debit_credit<<card_owner;
    //ui->labelAsiakas->setText(lista);

    reply->deleteLater();
    tapahtumaManager->deleteLater();
    QString site_url=url::getBaseUrl()+"transactions/";
    qDebug()<<site_url;
    QNetworkRequest request((site_url));
    //WEBTOKEN ALKU
    request.setRawHeader(QByteArray("Authorization"),(webToken));
    //WEBTOKEN LOPPU
    asiakasManager = new QNetworkAccessManager(this);
    connect(asiakasManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(asiakasSlot(QNetworkReply*)));
    reply = asiakasManager->get(request);

}

void tapahtumaWindow::asiakasSlot(QNetworkReply *reply)
{
    m_TableHeader<<"ID"<<"Kortin numero"<<"Summa"<<"Päivämäärä"<<"Tapahtuma"<<lista;
    ui->tapahtumaTable->setHorizontalHeaderLabels(m_TableHeader);
    QByteArray response_data=reply->readAll();
    qDebug()<<response_data;
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonArray json_array = json_doc.array();
    QJsonArray array = {};

        for (int i = 0; i < json_array.size();i++) {
            array.insert(i,json_array.at(i));
            QJsonValue items = json_array.at(i);
            QJsonObject obj = items.toObject();
            QString id_transactions = QString::number(obj["id_transactions"].toInt());
            QString card_numbers = QString::number(obj["card_number"].toInt());
            QString sums = QString::number(obj["sum"].toInt());
            QString dates = obj["date"].toString();
            QString descriptions = obj["description"].toString();

            if (card_numbers != tapahtumaWindow::card_number) {
                qDebug() << "ERIT: " << "listasta haettu numero: "<<card_numbers<< "käyttäjän numero: " <<card_number;
            } else {
                rowsOverTen++;
                qDebug() << rowsOverTen;
                ui->tapahtumaTable->setItem(totalRows, 0, new QTableWidgetItem(id_transactions));
                ui->tapahtumaTable->setItem(totalRows, 1, new QTableWidgetItem(card_numbers));
                ui->tapahtumaTable->setItem(totalRows, 2, new QTableWidgetItem(sums));
                ui->tapahtumaTable->setItem(totalRows, 3, new QTableWidgetItem(dates));
                ui->tapahtumaTable->setItem(totalRows, 4, new QTableWidgetItem(descriptions));
                ui->tapahtumaTable->resizeColumnsToContents();
                ui->tapahtumaTable->resizeRowsToContents();
                totalRows++;

                if (totalRows >= 10) {
                    maxAmmo++;
                    ui->tapahtumaTable->hideRow(rowsOverTen);
                }
            }
        }
        maxAmmo--;
        savedAmmo = maxAmmo;
        totalAmmoBackup = maxAmmo;
        ui->tapahtumaTable->setRowCount(totalRows);
        qDebug() << "over ten:" << maxAmmo;


    reply->deleteLater();
    asiakasManager->deleteLater();
    delay();
}
void tapahtumaWindow::on_closeButton_clicked()
{
    qDebug () << "sulje";
    this->close();
}
void tapahtumaWindow::on_backwardButton_clicked()
{
    qDebug() << "lastRowNumber entering back button: " <<lastVisibleRowNumber;
    qDebug() << "overTen entering back button: " <<maxAmmo;
    if (lastVisibleRowNumber == 10) {
        maxAmmo = totalAmmoBackup;
    } else {
        if (maxAmmo < 10) {
            for(int i = lastVisibleRowNumber;i >= lastVisibleRowNumber - maxAmmo;i--) {
                ui->tapahtumaTable->hideRow(i);
            }
            lastVisibleRowNumber -= maxAmmo;
            maxAmmo = lastVisibleRowNumber;
        } else if (maxAmmo >= 10){
            for(int i = lastVisibleRowNumber;i >= lastVisibleRowNumber - 10;i--) {
                ui->tapahtumaTable->hideRow(i);
            }
            lastVisibleRowNumber -= 10;
            maxAmmo = lastVisibleRowNumber;
        }
    }
    qDebug() << "lastRowNumber leaving back button: " <<lastVisibleRowNumber;
    qDebug() << "overTen leaving back button: " <<maxAmmo;
}
void tapahtumaWindow::on_forwardButton_clicked()
{
    if (lastVisibleRowNumber == 10 + maxAmmo) {
        lastVisibleRowNumber = 10 + maxAmmo;
    } else {
        if (maxAmmo >= 10) {
            for (int i = lastVisibleRowNumber; i < lastVisibleRowNumber + 10;i++) {
                ui->tapahtumaTable->showRow(i);
            }
            lastVisibleRowNumber = lastVisibleRowNumber + 10;
            maxAmmo = maxAmmo - 10;
            qDebug() << "lastRowNumber when overTen >= 10: " <<lastVisibleRowNumber;
        } else if (maxAmmo < 10) {
            for (int i = lastVisibleRowNumber; i < lastVisibleRowNumber + maxAmmo;i++) {
                ui->tapahtumaTable->showRow(i);
            }
            if (lastVisibleRowNumber != 10 + totalAmmoBackup) {
                lastVisibleRowNumber = lastVisibleRowNumber + maxAmmo;
                qDebug() << "lastRowNumber when overTen < 10: " <<lastVisibleRowNumber;
            }
        }
    }

  }



