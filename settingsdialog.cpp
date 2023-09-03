#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

#include "simpleconfig.hpp"

#include <QFileDialog>
#include <QStandardPaths>

SettingsDialog::SettingsDialog(QSharedPointer<SimpleConfig> simpleConfig, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
    , _simpleConfig(simpleConfig)
{
    ui->setupUi(this);

    setWindowTitle(tr("Settings"));

    connect(ui->closeButton, &QPushButton::clicked, this, &QDialog::accept);

    ui->useWorkspaceRoundingCheckBox->setChecked(
        _simpleConfig->get(SimpleConfig::USE_WORKSPACE_ROUNDING).toBool());
    connect(ui->useWorkspaceRoundingCheckBox,
            &QCheckBox::stateChanged,
            this,
            &SettingsDialog::useWorkspaceRoundingCheckStateChanged);

    ui->roundingComboBox->setCurrentIndex(_simpleConfig->get(SimpleConfig::CUSTOM_ROUNDING).toInt());
    connect(ui->roundingComboBox,
            &QComboBox::currentIndexChanged,
            this,
            &SettingsDialog::roundingChanged);

    ui->roundingMinutesComboBox->setCurrentIndex(
        _simpleConfig->get(SimpleConfig::CUSTOM_ROUNDING_MINUTES).toInt());
    connect(ui->roundingMinutesComboBox,
            &QComboBox::currentIndexChanged,
            this,
            &SettingsDialog::roundingMinutesChanged);

    ui->exportDefaultLocationEdit->setText(
        _simpleConfig
            ->get(SimpleConfig::EXPORT_DEFAULT_LOCATION,
                  QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation))
            .toString());
    connect(ui->exportDefaultLocationEdit,
            &QLineEdit::textChanged,
            this,
            &SettingsDialog::defaultLocationChanged);
    connect(ui->selectExportDefaultLocationButton,
            &QPushButton::clicked,
            this,
            &SettingsDialog::selectDefaultLocationClicked);

    ui->exportTitleEdit->setText(_simpleConfig->get(SimpleConfig::EXPORT_TITLE).toString());
    connect(ui->exportTitleEdit, &QLineEdit::textChanged, this, &SettingsDialog::titleChanged);

    ui->exportSubjectEdit->setText(_simpleConfig->get(SimpleConfig::EXPORT_SUBJECT).toString());
    connect(ui->exportSubjectEdit, &QLineEdit::textChanged, this, &SettingsDialog::subjectChanged);

    ui->exportCreatorEdit->setText(_simpleConfig->get(SimpleConfig::EXPORT_CREATOR).toString());
    connect(ui->exportCreatorEdit, &QLineEdit::textChanged, this, &SettingsDialog::creatorChanged);

    ui->exportCompanyEdit->setText(_simpleConfig->get(SimpleConfig::EXPORT_COMPANY).toString());
    connect(ui->exportCompanyEdit, &QLineEdit::textChanged, this, &SettingsDialog::companyChanged);

    ui->exportDescriptionEdit->setText(
        _simpleConfig->get(SimpleConfig::EXPORT_DESCRIPTION).toString());
    connect(ui->exportDescriptionEdit,
            &QLineEdit::textChanged,
            this,
            &SettingsDialog::descriptionChanged);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::useWorkspaceRoundingCheckStateChanged(int)
{
    _simpleConfig->set(SimpleConfig::USE_WORKSPACE_ROUNDING,
                       ui->useWorkspaceRoundingCheckBox->isChecked());
}
void SettingsDialog::roundingChanged(int index)
{
    _simpleConfig->set(SimpleConfig::CUSTOM_ROUNDING, index);
}
void SettingsDialog::roundingMinutesChanged(int index)
{
    _simpleConfig->set(SimpleConfig::CUSTOM_ROUNDING_MINUTES, index);
}

void SettingsDialog::selectDefaultLocationClicked()
{
    auto dir = QFileDialog::getExistingDirectory(this,
                                                 tr("Select export directory"),
                                                 ui->exportDefaultLocationEdit->text());
    if (dir.isEmpty()) {
        return;
    }
    ui->exportDefaultLocationEdit->setText(dir);
}
void SettingsDialog::defaultLocationChanged(const QString &defaultLocation)
{
    _simpleConfig->set(SimpleConfig::EXPORT_DEFAULT_LOCATION, defaultLocation);
}
void SettingsDialog::titleChanged(const QString &title)
{
    _simpleConfig->set(SimpleConfig::EXPORT_TITLE, title);
}
void SettingsDialog::subjectChanged(const QString &subject)
{
    _simpleConfig->set(SimpleConfig::EXPORT_SUBJECT, subject);
}
void SettingsDialog::creatorChanged(const QString &creator)
{
    _simpleConfig->set(SimpleConfig::EXPORT_CREATOR, creator);
}
void SettingsDialog::companyChanged(const QString &company)
{
    _simpleConfig->set(SimpleConfig::EXPORT_COMPANY, company);
}
void SettingsDialog::descriptionChanged(const QString &description)
{
    _simpleConfig->set(SimpleConfig::EXPORT_DESCRIPTION, description);
}
