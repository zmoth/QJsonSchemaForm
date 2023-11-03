#include "QJsonSchemaWidgets.h"

#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qcombobox.h>
#include <qdatetimeedit.h>
#include <qfiledialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qtoolbutton.h>
#include <qwidget.h>

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLine>
#include <QListWidget>
#include <QScrollArea>
#include <QSlider>
#include <QTabWidget>

#include "QJsonSchemaWidgetsFactory.h"
#include "ToggleSwitch.h"

QT_BEGIN_NAMESPACE
namespace QJsonSchemaForm {

#pragma region /*QJsonSchemaWidget*/

QJsonSchemaWidget::QJsonSchemaWidget(QWidget *parent) : QWidget(parent)
{
    // setStyleSheet(QString::fromUtf8("border:3px solid blue"));
}

void QJsonSchemaWidget::setSchema(const QJsonObject &schema)
{
    _schema = schema;  // 保存 schema

    processSchema(_schema);
};

#pragma endregion /*QJsonSchemaWidget*/

#pragma region /*QJsonSchemaObject*/

QJsonSchemaObject::QJsonSchemaObject(QWidget *parent) : QJsonSchemaWidget(parent)
{
    auto *layout = new QFormLayout(this);
    layout->setRowWrapPolicy(QFormLayout::WrapLongRows);
    // layout->setContentsMargins(0, 0, 0, 0);
    // layout->setSpacing(0);
}

QJsonSchemaObject::QJsonSchemaObject(const QJsonObject &schema, QWidget *parent) : QJsonSchemaObject(parent)
{
    setSchema(schema);
}

void QJsonSchemaObject::processSchema(const QJsonObject &schema)
{
    // 如果存在oneOf

    auto it = schema.find("oneOf");
    if (it != schema.end()) {
        auto *tabWidget = new QTabWidget(this);
        connect(tabWidget, &QTabWidget::currentChanged, [this]() { Q_EMIT changed(); });
        tabWidget->setObjectName("tabs");
        formLayout()->addWidget(tabWidget);

        auto array = it->toArray();
        int i = 0;
        for (auto v : array) {
            auto object = v.toObject();

            QString label{QString::number(i)};
            if (object.contains("title")) {
                label = object.find("title")->toString();
            }

            auto *newTab = QJsonSchemaWidgetsFactory::createWidget(object, this);

            if (newTab) {
                tabWidget->addTab(newTab, label);
            }
            i++;
        }

        return;
    }

    setOneOf(schema);
}

void QJsonSchemaObject::setOneOf(const QJsonObject &schema)
{
    auto *layout = formLayout();

    // 间距
    if (schema.contains("ui:spacing")) {
        auto spacing = schema.find("ui:spacing")->toInt();
        layout->setVerticalSpacing(spacing);
    }

    if (schema.contains("properties")) {
        auto properties = schema["properties"].toObject();
        for (auto it = properties.begin(); it != properties.end(); ++it) {
            const QString key = it.key();

            QJsonObject prop = it.value().toObject();

            // 标题
            QString title{key};
            {
                auto i = prop.find("title");
                if (i != prop.end()) {
                    title = i->toString(key);
                }
            }

            // 控件
            auto *widget = QJsonSchemaWidgetsFactory::createWidget(prop, this);
            widget->setWindowTitle(key);
            widget->setObjectName(key);

            // 是否显示
            bool visible{true};
            {
                auto i = prop.find("ui:visible");
                if (i != prop.end()) {
                    visible = i->toBool(true);
                }
            }
            widget->setVisible(visible);

            // 描述
            {
                auto i = prop.find("description");
                if (i != prop.end()) {
                    widget->setToolTip(i->toString());
                }
            }

            // 添加到map中管理
            widgetsMap[key] = {title, widget};
        }
    }

    // // ui格式
    // QString uiWidget = "rows";
    // if (schema.contains("ui:widget")) {
    //     uiWidget = schema.find("ui:widget")->toString();
    // }

    // 顺序
    if (schema.contains("ui:order")) {
        auto order = schema.find("ui:order")->toArray();
        for (auto valueRef : order) {
            QString name = valueRef.toString();
            if (widgetsMap.count(name)) {
                layout->addRow(widgetsMap[name].first, widgetsMap[name].second);
            }
        }
    } else {
        for (auto &widget : widgetsMap) {
            layout->addRow(widget.second.first, widget.second.second);
        }
    }
}

QJsonValue QJsonSchemaObject::getValue() const
{
    auto schema = getSchema();
    auto it = schema.find("oneOf");
    if (it != schema.end()) {
        auto *tabs = findChild<QTabWidget *>("tabs", Qt::FindDirectChildrenOnly);

        if (auto *currentTab = dynamic_cast<QJsonSchemaWidget *>(tabs->currentWidget())) {
            return currentTab->getValue().toObject();
        }
    }

    QJsonObject json;

    for (const auto &widget : widgetsMap) {
        json[widget.first] = widget.second.second->getValue();
    }
    return json;
}

void QJsonSchemaObject::setValue(const QJsonObject &json)
{
    auto schema = getSchema();
    auto it = schema.find("oneOf");
    if (it != schema.end()) {
        auto *tabs = findChild<QTabWidget *>("tabs", Qt::FindDirectChildrenOnly);

        if (auto *currentTab = dynamic_cast<QJsonSchemaWidget *>(tabs->currentWidget())) {
            QJsonSchemaWidgetsFactory::setValue(currentTab, json);
        }
        return;
    }

    for (const auto &key : json.keys()) {
        if (widgetsMap.count(key)) {
            QJsonSchemaWidgetsFactory::setValue(widgetsMap[key].second, json[key]);
        }
    }
}

[[nodiscard]] QFormLayout *QJsonSchemaObject::formLayout() const
{
    return qobject_cast<QFormLayout *>(layout());
}

void QJsonSchemaObject::setFormLayout(QFormLayout *layout)
{
    setLayout(layout);
}

#pragma endregion /*QJsonSchemaObject*/

#pragma region /*QJsonSchemaArray*/

QJsonSchemaArray::QJsonSchemaArray(QWidget *parent) : QJsonSchemaWidget(parent)
{
    {
        auto *l = new QFormLayout(this);

        propertiesLayout = new QFormLayout();

        {
            oneOf = new QComboBox(this);
        }

        auto *h = new QHBoxLayout();
        h->setAlignment(Qt::AlignRight);
        add = new QToolButton(this);
        add->setText("+");

        connect(add, &QToolButton::released, [this]() {
            auto i = oneOf->currentIndex();
            pushBack(oneOfArray.at(i).toObject());
            rebuild();
        });

        h->addWidget(oneOf);
        h->addWidget(add);

        l->addRow(propertiesLayout);
        l->addRow(h);

        this->setLayout(l);

        {
            listWidget = new QListWidget(this);
            l->addRow(listWidget);
        }
    }
}

QJsonSchemaArray::QJsonSchemaArray(const QJsonObject &schema, QWidget *parent) : QJsonSchemaArray(parent)
{
    setSchema(schema);
}

void QJsonSchemaArray::pushBack(const QJsonObject &o)
{
    auto *w = QJsonSchemaWidgetsFactory::createWidget(o, this);
    auto *h = new QHBoxLayout();

    h->addWidget(w);

    auto *up = new QToolButton(this);
    up->setArrowType(Qt::UpArrow);
    auto *down = new QToolButton(this);
    down->setArrowType(Qt::DownArrow);
    auto *del = new QToolButton(this);
    del->setText("✖");

    up->setVisible(!fixedOrder);
    down->setVisible(!fixedOrder);
    // del->setVisible(showButtons);

    up->setMaximumSize(25, 25);
    down->setMaximumSize(25, 25);
    del->setMaximumSize(25, 25);

    connect(down, &QAbstractButton::clicked, [w, this](bool) {
        size_t ro = 0;
        for (auto &x : items) {
            if (x.widget == w) {
                if (ro != items.size() - 1) {
                    std::swap(items[ro], items[ro + 1]);
                    rebuild();
                    Q_EMIT changed();
                    return;
                }
            }
            ro++;
        }
    });

    connect(up, &QAbstractButton::clicked, [w, this](bool) {
        size_t ro = 0;
        for (auto &x : items) {
            if (x.widget == w) {
                if (ro != 0) {
                    std::swap(items[ro - 1], items[ro]);
                    rebuild();
                    Q_EMIT changed();
                    return;
                }
            }
            ro++;
        }
    });

    connect(del, &QAbstractButton::clicked, [w, this](bool) {
        int ro = 0;
        for (auto &x : items) {
            if (x.widget == w) {
                delete x.widget;
                delete x.down;
                delete x.up;
                delete x.del;
                delete x.layout;
                items.erase(items.begin() + ro);
                Q_EMIT changed();
                return;
            }
            ro++;
        }
    });

    h->addWidget(up);
    h->addWidget(down);
    h->addWidget(del);

    items.push_back({w, h, up, down, del});
}

void QJsonSchemaArray::rebuild()
{
    auto *layout = propertiesLayout;

    while (layout->rowCount()) {
        auto h = layout->takeRow(layout->rowCount() - 1);
        bool found = false;
        for (auto &x : items) {
            if (h.fieldItem == x.layout) {
                found = true;
            }
        }
        if (!found) {
            delete h.fieldItem;
        }
    }

    for (auto &x : items) {
        x.del->setVisible(!fixedSize);
        layout->addRow(x.layout);
    }
}

void QJsonSchemaArray::processSchema(const QJsonObject &schema)
{
    oneOf->setVisible(false);
    add->setVisible(false);
    fixedSize = true;

    listWidget->clear();
    listWidget->hide();

    if (schema.contains("ui:spacing")) {
        auto spacing = schema.find("ui:spacing")->toInt();
        propertiesLayout->setVerticalSpacing(spacing);
        listWidget->setSpacing(spacing);
    }
    if (schema.contains("ui:fixedOrder")) {
        fixedOrder = schema.find("ui:fixedOrder")->toBool();
    }

    if (schema.contains("additionalItems")) {
        auto aItems = schema.find("additionalItems");
        if (aItems->isArray()) {
            oneOfArray = aItems->toArray();
        } else if (aItems->isObject()) {
            oneOfArray.push_back(aItems->toObject());
        }
        for (int i = 0; i < oneOfArray.size(); i++) {
            QString title = "ite_" + QString::number(i);
            auto obj = oneOfArray.at(i).toObject();
            if (obj.contains("title")) {
                title = obj.find("title")->toString();
            }
            oneOf->addItem(title);
        }

        if (oneOfArray.size() >= 2) {
            oneOf->setVisible(true);
        }
        add->setVisible(true);
        fixedSize = false;
    }

    bool uniqueItems = false;
    if (schema.contains("uniqueItems")) {
        uniqueItems = schema.find("uniqueItems")->toBool();
    }

    if (schema.contains("items")) {
        auto it = schema.find("items");

        if (it->isArray()) {
            auto p = it->toArray();

            int ro = 0;
            for (auto i : p) {
                pushBack(i.toObject());

                ro++;
            }
        } else if (it->isObject()) {
            auto p = it->toObject();

            if (p.find("type")->toString() == "string" && p.contains("enum") && uniqueItems) {
                int i = 0;
                for (auto v : p.find("enum")->toArray()) {
                    listWidget->insertItem(i++, v.toString());
                }
                listWidget->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
                listWidget->show();
            }
        }
    }
    rebuild();
}

QJsonValue QJsonSchemaArray::getValue() const
{
    QJsonArray o;
    if (listWidget->count() == 0) {
        for (const auto &x : items) {
            o.push_back(x.widget->getValue());
        }
    } else {
        for (auto *x : listWidget->selectedItems()) {
            o.push_back(x->text());
        }
    }
    return o;
}

void QJsonSchemaArray::setValue(QJsonArray data)
{
    auto mm = std::min(static_cast<size_t>(data.size()), items.size());
    for (int i = 0; i < mm; i++) {
        QJsonSchemaWidgetsFactory::setValue(items[i].widget, data[i]);
    }
}

#pragma endregion /*QJsonSchemaArray*/

#pragma region /*QJsonSchemaString*/

QJsonSchemaString::QJsonSchemaString(QWidget *parent) : QJsonSchemaWidget(parent)
{
    auto *layout = new QHBoxLayout(this);
}

QJsonSchemaString::QJsonSchemaString(const QJsonObject &schema, QWidget *parent) : QJsonSchemaString(parent)
{
    setSchema(schema);
}

void QJsonSchemaString::processSchema(const QJsonObject &schema)
{
    // 默认值
    QString defaultValue{};
    {
        auto iter = schema.find("default");
        if (iter != schema.end()) {
            defaultValue = iter->toString();
        }
    }

    // ui种类
    QString uiWidget{};
    {
        auto iter = schema.find("ui:widget");
        if (iter != schema.end()) {
            uiWidget = iter->toString();
        }
    }

    // 单选
    {
        auto iter = schema.find("enum");
        if (iter != schema.end()) {
            auto array = iter->toArray();

            auto *combo = new QComboBox(this);
            connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int) { Q_EMIT changed(); });
            _widget = combo;
            layout()->addWidget(_widget);

            for (auto value : array) {
                if (value.isString()) {
                    combo->addItem(value.toString());
                }
            }

            setValue(defaultValue);

            return;
        }
    }

    // 多行文本
    if (uiWidget == "text") {
        auto *textEdit = new QTextEdit(this);

        QFontMetrics metrics(textEdit->font());
        textEdit->setTabStopDistance(4 * metrics.averageCharWidth());

        connect(textEdit, &QTextEdit::textChanged, [this]() { Q_EMIT changed(); });
        _widget = textEdit;
        layout()->addWidget(_widget);

        setValue(defaultValue);

        return;
    }

    // 时间
    if (uiWidget == "date") {
        auto *dateEdit = new QDateEdit(this);
        connect(dateEdit, &QDateEdit::dateChanged, [this]() { Q_EMIT changed(); });
        _widget = dateEdit;
        layout()->addWidget(_widget);

        setValue(defaultValue);

        return;
    }

    // 单行文本
    auto *lineEdit = new QLineEdit(this);
    connect(lineEdit, &QLineEdit::textChanged, [this]() { Q_EMIT changed(); });
    _widget = lineEdit;
    layout()->addWidget(_widget);

    if (uiWidget == "file") {
        auto *fileButton = new QPushButton(this);
        connect(fileButton, &QPushButton::clicked, [this, lineEdit](bool) {
            // auto fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),
            // "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
            auto fileName = QFileDialog::getOpenFileName(this);
            lineEdit->setText(fileName);
            // _fileButton->setText(fileName);
        });
        layout()->addWidget(fileButton);

    } else if (uiWidget == "dir") {
        auto *dirButton = new QPushButton(this);
        connect(dirButton, &QPushButton::clicked, [this, lineEdit](bool) {
            // auto fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),
            // "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
            auto fileName = QFileDialog::getExistingDirectory(this);
            lineEdit->setText(fileName);
            // _fileButton->setText(fileName);
        });
        layout()->addWidget(dirButton);

    } else if (uiWidget == "color" || uiWidget == "colour") {
        auto *colorButton = new QPushButton(this);
        connect(colorButton, &QPushButton::clicked, [this, lineEdit](bool) {
            // auto fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),
            // "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
            // auto fileName = QFileDialog::getExistingDirectory(this);
            auto c = QColorDialog::getColor(Qt::white, this, "Choose a Color",
                                            QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
            int r, g, b, a;
            c.getRgb(&r, &g, &b, &a);
            char txt[25];
            int cx = std::snprintf(txt, 25, "#%02x%02x%02x%02x", r, g, b, a);
            lineEdit->setText(txt);
        });
        layout()->addWidget(colorButton);
    }

    setValue(defaultValue);
}

QJsonValue QJsonSchemaString::getValue() const
{
    if (auto *w = dynamic_cast<QLineEdit *>(_widget)) {
        return w->text();
    }
    if (auto *w = dynamic_cast<QTextEdit *>(_widget)) {
        return w->toPlainText();
    }
    if (auto *w = dynamic_cast<QComboBox *>(_widget)) {
        return w->currentText();
    }
    if (auto *w = dynamic_cast<QDateEdit *>(_widget)) {
        return w->date().toString(Qt::ISODate);
    }
    return {};
}

void QJsonSchemaString::setValue(const QString &v) const
{
    if (auto *w = dynamic_cast<QLineEdit *>(_widget)) {
        w->setText(v);
    } else if (auto *w = dynamic_cast<QTextEdit *>(_widget)) {
        w->setText(v);
    } else if (auto *w = dynamic_cast<QComboBox *>(_widget)) {
        w->setCurrentText(v);
    } else if (auto *w = dynamic_cast<QDateEdit *>(_widget)) {
        QDate d;
        d = QDate::fromString(v, Qt::ISODate);
        w->setDate(d);
    }
}

#pragma endregion /*QJsonSchemaString*/

#pragma region /*QJsonSchemaBoolean*/

QJsonSchemaBoolean::QJsonSchemaBoolean(QWidget *parent) : QJsonSchemaWidget(parent)
{
    auto *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignRight);
    // layout->setContentsMargins(0, 0, 0, 0);
    // layout->setSpacing(0);
}

QJsonSchemaBoolean::QJsonSchemaBoolean(const QJsonObject &schema, QWidget *parent) : QJsonSchemaBoolean(parent)
{
    setSchema(schema);
}

void QJsonSchemaBoolean::processSchema(const QJsonObject &schema)
{
    bool def{false};
    {
        auto it = schema.find("default");
        if (it != schema.end()) {
            def = it->toBool(false);
        }
    }
    QString wid = "checkbox";

    {
        auto it = schema.find("ui:widget");
        if (it != schema.end()) {
            wid = it->toString("checkbox");
        }
    }

    if (wid == "switch") {
        auto *s = new ToggleSwitch(8, 10, this);
        s->setChecked(def);
        _switch = s;

    } else {
        _switch = new QCheckBox(this);
        _switch->setChecked(def);
    }

    connect(_switch, &QAbstractButton::toggled, [this]() { Q_EMIT changed(); });

    layout()->addWidget(_switch);
}

QJsonValue QJsonSchemaBoolean::getValue() const
{
    return _switch->isChecked();
}

void QJsonSchemaBoolean::setValue(bool b)
{
    if (auto *s = dynamic_cast<ToggleSwitch *>(_switch)) {
        s->setChecked(b);
    } else if (_switch) {
        _switch->setChecked(b);
    }
}

#pragma endregion /*QJsonSchemaBoolean*/

#pragma region /*QJsonSchemaNumber*/

QJsonSchemaNumber::QJsonSchemaNumber(QWidget *parent) : QJsonSchemaWidget(parent)
{
    auto *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignRight);
}

QJsonSchemaNumber::QJsonSchemaNumber(const QJsonObject &schema, QWidget *parent) : QJsonSchemaNumber(parent)
{
    setSchema(schema);
}

void QJsonSchemaNumber::processSchema(const QJsonObject &schema)
{
    auto min = static_cast<double>(std::numeric_limits<int>::lowest());
    auto max = static_cast<double>(std::numeric_limits<int>::max());
    double def = 0.0;

    {
        // 有默认值时
        auto it = schema.find("default");
        if (it != schema.end()) {
            def = it->toDouble(0.0);
        }
    }
    {
        // 最小值
        auto it = schema.find("minimum");
        if (it != schema.end()) {
            min = it->toDouble(std::numeric_limits<double>::lowest());
        }
    }
    {
        // 最大值
        auto it = schema.find("maximum");
        if (it != schema.end()) {
            max = it->toDouble(std::numeric_limits<double>::max());
        }
    }

    QString wid = "updown";

    {
        auto it = schema.find("ui:widget");
        if (it != schema.end()) {
            wid = it->toString();
        }
    }

    // 下拉单选框
    {
        auto enumValue = schema.find("enum");
        auto enumName = schema.find("enumNames");
        if (enumValue != schema.end()) {
            auto e = enumName->toArray();

            auto *combo = new QComboBox(this);
            _widget = combo;
            combo->setObjectName("combo");
            connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int) { Q_EMIT changed(); });
            layout()->addWidget(combo);

            for (auto en : e) {
                if (en.isString()) {
                    combo->addItem(en.toString());
                }
            }

            return;
        }
    }

    {  // 水平布局
        auto *sliderBox = new QWidget(this);
        sliderBox->setObjectName("sliderBox");
        layout()->addWidget(sliderBox);

        auto *h = new QHBoxLayout(sliderBox);

        auto *slider = new QSlider(Qt::Horizontal, sliderBox);
        slider->setObjectName("slider");

        auto *spinBox = new QDoubleSpinBox(sliderBox);
        spinBox->setDecimals(7);
        spinBox->setObjectName("spinBox");
        _widget = spinBox;

        h->addWidget(slider, 10);
        h->addWidget(spinBox, 1);

        // spinBox 值改变时,更新
        connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double) { Q_EMIT changed(); });

        // silder 改变时，spinBox 变化
        connect(slider, &QSlider::valueChanged, [slider, spinBox](int i) {
            double t =
                static_cast<double>(i - slider->minimum()) / static_cast<double>(slider->maximum() - slider->minimum());
            double r = spinBox->minimum() * (1.0 - t) + spinBox->maximum() * t;
            spinBox->setValue(r);
        });

        // spinBox 改变时，silder 变化
        connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [spinBox, slider](double i) {
            double t = static_cast<double>(i - spinBox->minimum()) /
                       static_cast<double>(spinBox->maximum() - spinBox->minimum());
            double r = slider->minimum() * (1.0 - t) + slider->maximum() * t;
            slider->blockSignals(true);
            slider->setValue(static_cast<int>(r));
            slider->blockSignals(false);
        });

        if (wid == "range") {
            spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
            slider->show();
            spinBox->show();
        } else {
            spinBox->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
            slider->hide();
            spinBox->show();
        }

        spinBox->setRange(min, max);
        slider->setRange(0, 0xFFFFFF);
        spinBox->setValue(def);

        if (schema.find("type")->toString() == "integer") {
            spinBox->setDecimals(0);
        }
    }
}

QJsonValue QJsonSchemaNumber::getValue() const
{
    if (auto *combo = dynamic_cast<QComboBox *>(_widget)) {
        return combo->currentIndex();
    }
    return dynamic_cast<QDoubleSpinBox *>(_widget)->value();
}

void QJsonSchemaNumber::setValue(double v)
{
    if (auto *combo = dynamic_cast<QComboBox *>(_widget)) {
        combo->setCurrentIndex(static_cast<int>(v));
    } else if (auto *box = dynamic_cast<QDoubleSpinBox *>(_widget)) {
        box->setValue(v);
    }
}

#pragma endregion /*QJsonSchemaNumber*/

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE