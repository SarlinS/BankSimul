#include "talletuswindow.h"
#include "ui_talletuswindow.h"
#include "bankwindow.h"

talletusWindow::talletusWindow(QByteArray token,QString cardNumber,bool cardType,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::talletusWindow)
{
    ui->setupUi(this);
    talletusWindow::setWindowState(Qt::WindowMaximized);
    ui->labelTalletus->hide();
    card_number = cardNumber;
    webToken=token;

    ui->timer->setPalette(Qt::red);
    ui->timer->setAutoFillBackground(true);
    QPalette Pal = ui->timer->palette();
    Pal.setColor(QPalette::Normal, QPalette::WindowText, Qt::red);
    Pal.setColor(QPalette::Normal, QPalette::Window, Qt::black);
    ui->timer->setPalette(Pal);

    if(cardType==true)//debit käytössä = false , credit käytössä = true
    {
        credit=true;
    }
    else
    {
        credit=false;
    }
    QString site_url=url::getBaseUrl()+"cards/"+card_number;
    QNetworkRequest request((site_url));
    qDebug()<<site_url;
    //WEBTOKEN ALKU
    request.setRawHeader(QByteArray("Authorization"),(webToken));
    //WEBTOKEN LOPPU
    asiakasManager = new QNetworkAccessManager(this);
    connect(asiakasManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(asiakasSlot(QNetworkReply*)));
    reply = asiakasManager->get(request);
}

talletusWindow::~talletusWindow()
{
    delete ui;
}

void talletusWindow::delay()
{

    int afkTimer=1;
    QTime dieTime= QTime::currentTime().addSecs(afkTimer);
     while (QTime::currentTime() < dieTime)
         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void talletusWindow::talletusDelay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void talletusWindow::asiakasSlot(QNetworkReply *reply)
{
    aika = 10;
    QByteArray response_data=reply->readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonObject json_obj = json_doc.object();
    account = QString::number(json_obj["id_account"].toInt());
    qDebug()<<"account"<<account;

    QString site_url=url::getBaseUrl()+"accounts/"+account;
        qDebug()<<site_url;
    QNetworkRequest request((site_url));
    //WEBTOKEN ALKU
    request.setRawHeader(QByteArray("Authorization"),(webToken));
    //WEBTOKEN LOPPU
    saldoManager = new QNetworkAccessManager(this);
    connect(saldoManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(saldoSlot(QNetworkReply*)));
    reply = saldoManager->get(request);
}

void talletusWindow::saldoSlot(QNetworkReply *reply)
{
    QByteArray response_data=reply->readAll();
    qDebug()<<response_data;
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonObject json_obj = json_doc.object();

    saldo=QString::number(json_obj["debit_balance"].toDouble());
    credit_limit=QString::number(json_obj["credit_limit"].toDouble());
    qDebug()<<"saldo"<<saldo;
    x = saldo.toDouble();
    ui->talletusLabel->setText("Syötä talletettava määrä");

    for (aika = 10; aika >= 0; aika--) {
        delay();
        ui->timer->display(aika);
    }
    bankwindow *main = new bankwindow(webToken,card_number,credit);
    main->show();
    close();
}

void talletusWindow::on_talletaButton_clicked()
{
    if(credit==false){
    aika = 10;
    ui->talletusLabel->clear();
    sum=ui->lineEditMaara->text();
    ui->lineEditMaara->clear();
    qDebug()<<"summa"<<sum;
    double y = sum.toDouble();
    talletus=x + y;
    qDebug()<<"talletus"<<talletus;
    QJsonObject jsonObj;
    jsonObj.insert("debit_balance",talletus);
    jsonObj.insert("credit_limit",credit_limit);
    QString uusiSaldo = QString::number(talletus);
    ui->saldoLabel->setText("Saldo: " + uusiSaldo);
    ui->summaLabel->setText("Talletettu summa: " + sum);
    QString site_url=url::getBaseUrl()+"/accounts/"+account;
    QNetworkRequest request((site_url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    //WEBTOKEN ALKU
    request.setRawHeader(QByteArray("Authorization"),(webToken));
    //WEBTOKEN LOPPU
    talletusManager = new QNetworkAccessManager(this);
    connect(talletusManager, SIGNAL(finished (QNetworkReply*)), this, SLOT(talletusSlot(QNetworkReply*)));
    reply = talletusManager->put(request, QJsonDocument(jsonObj).toJson());
    ui->labelTalletus->show();

    for (int i = 5; i >= 1;i--) {
        QString info ="Talletettu summa:" + sum + "\n" + "Uusi saldo: "+ uusiSaldo + "\nSuljetaan " +
        QString::number(i);
        ui->labelTalletus->document()->setPlainText(info);
        talletusDelay();
    }}
    else{
        close();
    }

    bankwindow *main = new bankwindow(webToken,card_number,credit);
    main->show();
    close();

}

void talletusWindow::talletusSlot(QNetworkReply *reply)
{
    response_data=reply->readAll();
    reply->deleteLater();
    talletusManager->deleteLater();
}

void talletusWindow::on_peruutaButton_clicked()
{
    bankwindow *bank = new bankwindow(webToken, card_number,credit);
    bank->show();
    close();
}

void talletusWindow::on_pushButton_1_clicked()
{
    aika = 10;
    ui->lineEditMaara->setText(ui->lineEditMaara->text()+ "1");
    delay();
}

void talletusWindow::on_pushButton_2_clicked()
{
    aika = 10;
    ui->lineEditMaara->setText(ui->lineEditMaara->text()+ "2");
    delay();
}


void talletusWindow::on_pushButton_3_clicked()
{
    aika = 10;
    ui->lineEditMaara->setText(ui->lineEditMaara->text()+ "3");
    delay();
}


void talletusWindow::on_pushButton_4_clicked()
{
    aika = 10;
    ui->lineEditMaara->setText(ui->lineEditMaara->text()+ "4");
    delay();
}


void talletusWindow::on_pushButton_5_clicked()
{
    aika = 10;
    ui->lineEditMaara->setText(ui->lineEditMaara->text()+ "5");
    delay();
}


void talletusWindow::on_pushButton_6_clicked()
{
    aika = 10;
    ui->lineEditMaara->setText(ui->lineEditMaara->text()+ "6");
    delay();
}


void talletusWindow::on_pushButton_7_clicked()
{
    aika = 10;
    ui->lineEditMaara->setText(ui->lineEditMaara->text()+ "7");
    delay();
}


void talletusWindow::on_pushButton_8_clicked()
{
    aika = 10;
    ui->lineEditMaara->setText(ui->lineEditMaara->text()+ "8");
    delay();
}


void talletusWindow::on_pushButton_9_clicked()
{
    aika = 10;
    ui->lineEditMaara->setText(ui->lineEditMaara->text()+ "9");
    delay();
}


void talletusWindow::on_pushButton_0_clicked()
{
    aika = 10;
    ui->lineEditMaara->setText(ui->lineEditMaara->text()+ "0");
    delay();
}


void talletusWindow::on_pyyhiButton_clicked()
{
    ui->lineEditMaara->backspace();
}

