#include "dealDialog.h"

DealDialog::DealDialog(QWidget *parent) : QDialog(parent) {
    qDebug() << "LogFunc: DealDialog()";
    
    setWindowTitle("Make Deal");

    amountLineEdit = new QLineEdit(this);
    exchangeRateLineEdit = new QLineEdit(this);
    dealTypeComboBox = new QComboBox(this);
    dealTypeComboBox->addItem("buy");
    dealTypeComboBox->addItem("sell");

    submitButton = new QPushButton("Submit", this);

    layout = new QFormLayout(this);
    layout->addRow("Amount:", amountLineEdit);
    layout->addRow("Exchange Rate:", exchangeRateLineEdit);
    layout->addRow("Deal Type:", dealTypeComboBox);
    layout->addWidget(submitButton);

    connect(submitButton, &QPushButton::clicked, this, &DealDialog::handleSubmit);
    setFixedSize(300, 150);
}

void DealDialog::handleSubmit() {
    qDebug() << "LogFunc: handleSubmit()";

    emit dealSubmitted(amountLineEdit->text(), exchangeRateLineEdit->text(), dealTypeComboBox->currentText());
    close();
}

// #include "dealDialog.moc"