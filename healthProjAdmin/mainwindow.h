#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QDebug>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include <QInputDialog>
#include <QtSql/QSqlError>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    QSqlQuery *query;
    QSqlTableModel *patimodel = new QSqlTableModel;
    QSqlTableModel *devimodel = new QSqlTableModel;
    QSqlQueryModel *patidevimodel = new QSqlQueryModel;
    QString bindInfoSQL = "select id, facility.facilityid,patient.patientid,serialflag,intnetflag,idcardNum,name,age,sex"
                          " from facipati, facility, patient"
                          " where facility.facilityid=facipati.facilityid and patient.patientid=facipati.patientid;";
    bool haveLogged = false;

private slots:
    void on_loginPageBtn_clicked();

    void on_patientInfoPageBtn_clicked();

    void on_deviceBindBtn_clicked();

    void on_wavePageBtn_clicked();

    void on_otherDataBtn_clicked();

    void on_pushButton_clicked();

    void on_insertBtn_clicked();

    void on_updateBtn_clicked();

    void on_modifyBtn_clicked();

    void on_patientTable_doubleClicked(const QModelIndex &index);

    void on_deleteBtn_clicked();

    void on_bindBtn_clicked();

    void on_unBindBtn_clicked();

    void on_updateBindBtn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
