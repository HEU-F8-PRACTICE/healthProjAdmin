#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db.setHostName("127.0.0.1");
    db.setDatabaseName("justfortest");
    db.setUserName("root");
    db.setPassword("465341123");
    if(!db.open())
    {
        QMessageBox::warning(this, "数据库连接失败", db.lastError().text());
    }
    else
    {
        query = new QSqlQuery(db);
        patimodel->setTable("patient");
    }
    ui->patientTable->setModel(patimodel);
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_loginPageBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_patientInfoPageBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_deviceBindBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_wavePageBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_otherDataBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::on_pushButton_clicked()
{
    auto account = ui->accountEdit->text();
    auto password = ui->passwordEdit->text();
    query->exec(QString("select * from doctorinfo where account='%1' and password='%2';").arg(account).arg(password));
    if(query->next())
    {
        haveLogged = true;
        ui->statusLabel->setText("状态：已登录");
    }
    else
    {
        QMessageBox::critical(this, "登录失败", "用户名或密码错误");
    }
}

void MainWindow::on_insertBtn_clicked()
{
    if(haveLogged)
    {
        auto patiname = ui->nameEdit->text();
        auto patiSex = ui->sexEdit->text();
        auto patiAge = ui->ageEdit->text();
        auto patiID = ui->idNumEdit->text();
        if(!query->exec(QString("insert into patient(idcardNum, name, age, sex) values('%1', '%2', %3, '%4');")
                    .arg(patiID).arg(patiname).arg(patiAge).arg(patiSex)))
        {
            QMessageBox::critical(this, "插入信息失败", query->lastError().text());
        }
        patimodel->select();
    }
    else
        QMessageBox::information(this, "尚未登录", "请先登录！");
}

void MainWindow::on_updateBtn_clicked()
{
    if(haveLogged)
    {
        patimodel->select();
    }
    else
        QMessageBox::information(this, "尚未登录", "请先登录！");
}

void MainWindow::on_patientTable_doubleClicked(const QModelIndex &index)
{
    if(haveLogged)
    {
        bool isOk;
        auto modifiedText = QInputDialog::getText(this, "修改内容", "修改为：", QLineEdit::Normal,index.data().toString(), &isOk);
        if(!isOk)
            return;
        patimodel->setData(patimodel->index(index.row(),index.column()),modifiedText);
        patimodel->submitAll();
        patimodel->select();
        patimodel->setData(patimodel->index(index.row(), index.column()), modifiedText);
    }
    else
        QMessageBox::information(this, "尚未登录", "请先登录！");
}

void MainWindow::on_modifyBtn_clicked()
{
    auto index = ui->patientTable->currentIndex();
    if(haveLogged)
    {
        bool isOk;
        auto modifiedText = QInputDialog::getText(this, "修改内容", "修改为：", QLineEdit::Normal,index.data().toString(), &isOk);
        if(!isOk)
            return;
        patimodel->setData(patimodel->index(index.row(),index.column()),modifiedText);
        patimodel->submitAll();
        patimodel->select();
        patimodel->setData(patimodel->index(index.row(), index.column()), modifiedText);
    }
    else
        QMessageBox::information(this, "尚未登录", "请先登录！");
}

void MainWindow::on_deleteBtn_clicked()
{
    int row = ui->patientTable->currentIndex().row();//记录当前选择行
    if(QMessageBox::Yes == QMessageBox::question(this,"Make Sure","确定删除第"+QString::number(row+1)+"行吗？",QMessageBox::Yes | QMessageBox::No , QMessageBox::Yes))
    {
        patimodel->removeRow(row);
        patimodel->submitAll();
        patimodel->select();
    }
}
