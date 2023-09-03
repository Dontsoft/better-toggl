#include "switchworkspacedialog.hpp"
#include "ui_switchworkspacedialog.h"

#include <QListWidgetItem>
#include <QPushButton>

#include "globalstorage.hpp"
#include "simpleconfig.hpp"

#include "toggl/apiinterface.hpp"

SwitchWorkspaceDialog::SwitchWorkspaceDialog(QSharedPointer<GlobalStorage> globalStorage,
                                             QSharedPointer<SimpleConfig> simpleConfig,
                                             QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SwitchWorkspaceDialog)
    , _globalStorage(globalStorage)
    , _simpleConfig(simpleConfig)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Switch Workspace"));

    connect(ui->buttonBox->button(QDialogButtonBox::Ok),
            &QPushButton::clicked,
            this,
            &SwitchWorkspaceDialog::switchWorkspaceButtonClicked);

    connect(ui->buttonBox->button(QDialogButtonBox::Cancel),
            &QPushButton::clicked,
            this,
            &SwitchWorkspaceDialog::reject);

    auto me = _globalStorage->me();
    auto workspaceId = qvariant_cast<int>(
        _simpleConfig->get(SimpleConfig::WORKSPACE,
                           me.get<int>(toggl::Me::DEFAULT_WORKSPACE_ID).value_or(-1)));
    auto workspaces = _globalStorage->workspaces();
    for (const toggl::Workspace &workspace : workspaces) {
        const auto name = workspace.get<QString>(toggl::Workspace::NAME).value_or("");
        const auto id = workspace.get<int>(toggl::Workspace::ID).value_or(-1);
        auto item = new QListWidgetItem(QString("%1 (%2)").arg(name).arg(id));
        item->setData(Qt::UserRole, id);
        if (id == workspaceId) {
            item->setCheckState(Qt::Checked);
            item->setSelected(true);
        }
        ui->workspaceListWidget->addItem(item);
    }
}

SwitchWorkspaceDialog::~SwitchWorkspaceDialog()
{
    delete ui;
}

void SwitchWorkspaceDialog::switchWorkspaceButtonClicked()
{
    if (ui->workspaceListWidget->selectedItems().empty()) {
        return;
    }
    auto selectedItem = ui->workspaceListWidget->selectedItems().first();
    int id = qvariant_cast<int>(selectedItem->data(Qt::UserRole));
    _simpleConfig->set(SimpleConfig::WORKSPACE, id);
    accept();
}
