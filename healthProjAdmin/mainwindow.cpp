#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->ecgWaveDisplay->y0 = 0;
    ui->ecgWaveDisplay->yScale = 4096;
    ui->ecgWaveDisplay->color = Qt::green;
    ui->ecgWaveDisplay->text = "ECG";
    ui->spo2WaveDisplay->y0 = 0;
    ui->spo2WaveDisplay->yScale = 256;
    ui->spo2WaveDisplay->color = Qt::red;
    ui->spo2WaveDisplay->text = "SPO2";
    ui->bpWaveDisplay->y0 = 0;
    ui->bpWaveDisplay->yScale = 256;
    ui->bpWaveDisplay->color = Qt::yellow;
    ui->bpWaveDisplay->text = "BP";
    connect(&updatetimer, &QTimer::timeout, this, &MainWindow::updateCharts);
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
        patimodel->select();
        devimodel->select();
        patidevimodel->setQuery(bindInfoSQL);

        patidevimodel->setHeaderData(0, Qt::Horizontal, "ID");
        patidevimodel->setHeaderData(1, Qt::Horizontal, "设备ID");
        patidevimodel->setHeaderData(2, Qt::Horizontal, "患者ID");
        patidevimodel->setHeaderData(3, Qt::Horizontal, "序列号");
        patidevimodel->setHeaderData(4, Qt::Horizontal, "时间");
        patidevimodel->setHeaderData(5, Qt::Horizontal, "身份证号");
        patidevimodel->setHeaderData(6, Qt::Horizontal, "姓名");
        patidevimodel->setHeaderData(7, Qt::Horizontal, "年龄");
        patidevimodel->setHeaderData(8, Qt::Horizontal, "性别");
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

void MainWindow::on_bindBtn_clicked()
{
    auto patiindex = patimodel->index(ui->patiBindTable->currentIndex().row(), 0).data().toString();
    auto deviindex = devimodel->index(ui->deviceBindTable->currentIndex().row(), 0).data().toString();
    if(!query->exec(QString("insert into facipati(facilityid, patientid) values(%1, %2)").arg(deviindex).arg(patiindex)))
        QMessageBox::critical(this, "失败", query->lastError().text());
    patidevimodel->setQuery(bindInfoSQL);

    patidevimodel->setHeaderData(0, Qt::Horizontal, "ID");
    patidevimodel->setHeaderData(1, Qt::Horizontal, "设备ID");
    patidevimodel->setHeaderData(2, Qt::Horizontal, "患者ID");
    patidevimodel->setHeaderData(3, Qt::Horizontal, "序列号");
    patidevimodel->setHeaderData(4, Qt::Horizontal, "时间");
    patidevimodel->setHeaderData(5, Qt::Horizontal, "身份证号");
    patidevimodel->setHeaderData(6, Qt::Horizontal, "姓名");
    patidevimodel->setHeaderData(7, Qt::Horizontal, "年龄");
    patidevimodel->setHeaderData(8, Qt::Horizontal, "性别");
}

void MainWindow::on_unBindBtn_clicked()
{
    auto id = patidevimodel->index(ui->patideviBindTable->currentIndex().row(), 0).data().toString();
    if(!query->exec(QString("delete from facipati where facilityid=%1").arg(id)))
        QMessageBox::critical(this, "失败", query->lastError().text());
    patidevimodel->setQuery(bindInfoSQL);

    patidevimodel->setHeaderData(0, Qt::Horizontal, "ID");
    patidevimodel->setHeaderData(1, Qt::Horizontal, "设备ID");
    patidevimodel->setHeaderData(2, Qt::Horizontal, "患者ID");
    patidevimodel->setHeaderData(3, Qt::Horizontal, "序列号");
    patidevimodel->setHeaderData(4, Qt::Horizontal, "时间");
    patidevimodel->setHeaderData(5, Qt::Horizontal, "身份证号");
    patidevimodel->setHeaderData(6, Qt::Horizontal, "姓名");
    patidevimodel->setHeaderData(7, Qt::Horizontal, "年龄");
    patidevimodel->setHeaderData(8, Qt::Horizontal, "性别");
}

void MainWindow::on_updateBindBtn_clicked()
{
    if(haveLogged)
    {
        patimodel->select();
        patidevimodel->setQuery(bindInfoSQL);

        patidevimodel->setHeaderData(0, Qt::Horizontal, "ID");
        patidevimodel->setHeaderData(1, Qt::Horizontal, "设备ID");
        patidevimodel->setHeaderData(2, Qt::Horizontal, "患者ID");
        patidevimodel->setHeaderData(3, Qt::Horizontal, "序列号");
        patidevimodel->setHeaderData(4, Qt::Horizontal, "时间");
        patidevimodel->setHeaderData(5, Qt::Horizontal, "身份证号");
        patidevimodel->setHeaderData(6, Qt::Horizontal, "姓名");
        patidevimodel->setHeaderData(7, Qt::Horizontal, "年龄");
        patidevimodel->setHeaderData(8, Qt::Horizontal, "性别");
        devimodel->select();
    }
    else
        QMessageBox::information(this, "尚未登录", "请先登录！");
}

void MainWindow::on_updateWaveBtn_clicked()
{
    if(haveLogged)
    {
        updatetimer.stop();
        if(ui->realtime->isChecked())
        {
            updateCharts();
            updatetimer.start(ui->spinBox->text().toInt()*1000);
        }
        else
        {
            QStringList tempString;
            auto patiindex = devimodel->index(ui->waveDeviTable->currentIndex().row(), 0).data().toString();
            auto timeFilter = ui->beginTime->text();
            query->exec(QString("select ecgdata from ecgwave where facilityid=%1 and inserttime<'%2' order by inserttime desc limit 0,4").arg(patiindex).arg(timeFilter));
            ecgData.clear();
            while(query->next())
            {
                tempString = query->value(0).toString().split(".");
                for(int i=0;i<tempString.size();i++)
                {
                    ecgData.append(tempString[i].toInt());
                }
            }
            query->exec(QString("select spo2data from spo2wave where facilityid=%1 and inserttime<'%2' order by inserttime desc limit 0,2").arg(patiindex).arg(timeFilter));
            spo2Data.clear();
            while(query->next())
            {
                tempString = query->value(0).toString().split(".");
                for(int i=0;i<tempString.size();i++)
                {
                    spo2Data.append(tempString[i].toInt());
                }
            }
            query->exec(QString("select bpdata from bpwave where facilityid=%1 and inserttime<'%2' order by inserttime desc limit 0,1").arg(patiindex).arg(timeFilter));
            bpData.clear();
            while(query->next())
            {
                tempString = query->value(0).toString().split(".");
                for(int i=0;i<tempString.size();i++)
                {
                    bpData.append(tempString[i].toInt());
                }
            }
            ui->ecgWaveDisplay->sendData(ecgData);
            ui->spo2WaveDisplay->sendData(spo2Data);
            ui->bpWaveDisplay->sendData(bpData);
        }
    }
    else
        QMessageBox::information(this, "尚未登录", "请先登录！");
}

void MainWindow::updateCharts()
{
    qDebug() << "Querying";
    QStringList tempString;
    auto patiindex = devimodel->index(ui->waveDeviTable->currentIndex().row(), 0).data().toString();
    query->exec(QString("select ecgdata from ecgwave where facilityid=%1 order by inserttime desc limit 0,4").arg(patiindex));
    ecgData.clear();
    while(query->next())
    {
        tempString = query->value(0).toString().split(".");
        for(int i=0;i<tempString.size();i++)
        {
            ecgData.append(tempString[i].toInt());
        }
    }
    query->exec(QString("select spo2data from spo2wave where facilityid=%1 order by inserttime desc limit 0,2").arg(patiindex));
    spo2Data.clear();
    while(query->next())
    {
        tempString = query->value(0).toString().split(".");
        for(int i=0;i<tempString.size();i++)
        {
            spo2Data.append(tempString[i].toInt());
        }
    }
    query->exec(QString("select bpdata from bpwave where facilityid=%1 order by inserttime desc limit 0,1").arg(patiindex));
    bpData.clear();
    while(query->next())
    {
        tempString = query->value(0).toString().split(".");
        for(int i=0;i<tempString.size();i++)
        {
            bpData.append(tempString[i].toInt());
        }
    }
    ui->ecgWaveDisplay->sendData(ecgData);
    ui->spo2WaveDisplay->sendData(spo2Data);
    ui->bpWaveDisplay->sendData(bpData);
}

void MainWindow::on_otherUpdateBtn_clicked()
{
    auto faciIndex = devimodel->index(ui->otherDeviTable->currentIndex().row(), 0).data().toString();
    auto beginTime = ui->dateTimeEdit->text();
    auto endTime = ui->dateTimeEdit_2->text();
    if(ui->hrRadio->isChecked())
    {
        qDebug() << QString("facilityid=%1 and inserttime>'%2' and inserttime<'%3'").arg(faciIndex).arg(beginTime).arg(endTime);
        model1->setFilter(QString("facilityid=%1 and inserttime>'%2' and inserttime<'%3'").arg(faciIndex).arg(beginTime).arg(endTime));
        model1->select();
        ui->tableView->setModel(model1);
    }
    else if(ui->stRadio->isChecked())
    {
        model2->setFilter(QString("facilityid=%1 and inserttime>'%2' and inserttime<'%3'").arg(faciIndex).arg(beginTime).arg(endTime));
        model2->select();
        ui->tableView->setModel(model2);
    }
    else if(ui->nibpRadio->isChecked())
    {
        model3->setFilter(QString("facilityid=%1 and inserttime>'%2' and inserttime<'%3'").arg(faciIndex).arg(beginTime).arg(endTime));
        model3->select();
        ui->tableView->setModel(model3);
    }
    else if(ui->spo2Radio->isChecked())
    {
        model4->setFilter(QString("facilityid=%1 and inserttime>'%2' and inserttime<'%3'").arg(faciIndex).arg(beginTime).arg(endTime));
        model4->select();
        ui->tableView->setModel(model4);
    }
    else if(ui->tempRadio->isChecked())
    {
        model5->setFilter(QString("facilityid=%1 and inserttime>'%2' and inserttime<'%3'").arg(faciIndex).arg(beginTime).arg(endTime));
        model5->select();
        ui->tableView->setModel(model5);
    }
    else if(ui->respRadio->isChecked())
    {
        model6->setFilter(QString("facilityid=%1 and inserttime>'%2' and inserttime<'%3'").arg(faciIndex).arg(beginTime).arg(endTime));
        model6->select();
        ui->tableView->setModel(model6);
    }
    else if(ui->co2Radio->isChecked())
    {
        model7->setFilter(QString("facilityid=%1 and inserttime>'%2' and inserttime<'%3'").arg(faciIndex).arg(beginTime).arg(endTime));
        model7->select();
        ui->tableView->setModel(model7);
    }
}

void MainWindow::on_connectBtn_clicked()
{
    auto hostName = ui->dbIPEdit->text();
    auto dbName = ui->dbnameEdit->text();
    auto dbAccount = ui->dbaccountEdit->text();
    auto dbPassword = ui->dbPasswordEdit->text();
    //db.setHostName("49.232.171.182");
    //db.setDatabaseName("justfortest");
    //db.setUserName("justfortest");
    //db.setPassword("DHrMiX74SPCXaAjh");
    db.setHostName(hostName);
    db.setDatabaseName(dbName);
    db.setUserName(dbAccount);
    db.setPassword(dbPassword);
    if(!db.open())
    {
        QMessageBox::warning(this, "数据库连接失败", db.lastError().text());
    }
    else
    {
        query = new QSqlQuery(db);
        patimodel->setTable("patient");
        devimodel->setTable("facility");
        ui->pushButton->setEnabled(true);
        ui->patientTable->setModel(patimodel);
        ui->patiBindTable->setModel(patimodel);
        ui->deviceBindTable->setModel(devimodel);
        ui->patideviBindTable->setModel(patidevimodel);
        ui->waveDeviTable->setModel(devimodel);
        ui->otherDeviTable->setModel(devimodel);
        model1->setTable("hr");
        model2->setTable("st");
        model3->setTable("nibp");
        model4->setTable("spo2");
        model5->setTable("temp");
        model6->setTable("resp");
        model7->setTable("co2");

        patimodel->setHeaderData(0, Qt::Horizontal, "ID");
        patimodel->setHeaderData(1, Qt::Horizontal, "身份证号");
        patimodel->setHeaderData(2, Qt::Horizontal, "姓名");
        patimodel->setHeaderData(3, Qt::Horizontal, "年龄");
        patimodel->setHeaderData(4, Qt::Horizontal, "性别");

        devimodel->setHeaderData(0, Qt::Horizontal, "ID");
        devimodel->setHeaderData(1, Qt::Horizontal, "序列号");
        devimodel->setHeaderData(2, Qt::Horizontal, "添加时间");

        model1->setHeaderData(0, Qt::Horizontal, "设备ID");
        model1->setHeaderData(1, Qt::Horizontal, "时间");
        model1->setHeaderData(2, Qt::Horizontal, "数据");

        model2->setHeaderData(0, Qt::Horizontal, "设备ID");
        model2->setHeaderData(1, Qt::Horizontal, "时间");
        model2->setHeaderData(2, Qt::Horizontal, "ST1");
        model2->setHeaderData(3, Qt::Horizontal, "ST2");
        model2->setHeaderData(4, Qt::Horizontal, "PVC");

        model3->setHeaderData(0, Qt::Horizontal, "设备ID");
        model3->setHeaderData(1, Qt::Horizontal, "时间");
        model3->setHeaderData(2, Qt::Horizontal, "高压");
        model3->setHeaderData(3, Qt::Horizontal, "低压");
        model3->setHeaderData(4, Qt::Horizontal, "均值");

        model4->setHeaderData(0, Qt::Horizontal, "设备ID");
        model4->setHeaderData(1, Qt::Horizontal, "时间");
        model4->setHeaderData(2, Qt::Horizontal, "SPO2");
        model4->setHeaderData(3, Qt::Horizontal, "脉率");

        model5->setHeaderData(0, Qt::Horizontal, "设备ID");
        model5->setHeaderData(1, Qt::Horizontal, "时间");
        model5->setHeaderData(2, Qt::Horizontal, "T1");
        model5->setHeaderData(3, Qt::Horizontal, "T2");
        model5->setHeaderData(4, Qt::Horizontal, "TD");

        model6->setHeaderData(0, Qt::Horizontal, "设备ID");
        model6->setHeaderData(1, Qt::Horizontal, "时间");
        model6->setHeaderData(2, Qt::Horizontal, "数据");

        model7->setHeaderData(0, Qt::Horizontal, "设备ID");
        model7->setHeaderData(1, Qt::Horizontal, "时间");
        model7->setHeaderData(2, Qt::Horizontal, "CO2");
        model7->setHeaderData(3, Qt::Horizontal, "AWRR");
        model7->setHeaderData(4, Qt::Horizontal, "INS");
    }
}
