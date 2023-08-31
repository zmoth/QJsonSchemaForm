#include "QJsonSchemaWidgets.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLine>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolButton>
#include <QWidget>

#include "QJsonSchemaWidgetsFactory.h"
#include "ToggleSwitch.h"

QT_BEGIN_NAMESPACE
namespace QJsonSchemaForm {

#pragma region /*QJsonSchemaWidget*/

QJsonSchemaWidget::QJsonSchemaWidget(QWidget *parent) : QWidget(parent)
{
    // setStyleSheet(QString::fromUtf8("border:3px solid blue"));
}

#pragma endregion /*QJsonSchemaWidget*/

#pragma region /*QJsonSchemaObject*/

QJsonSchemaObject::QJsonSchemaObject(QJsonSchemaWidget *parent) : QJsonSchemaWidget(parent)
{
    auto *layout = new QFormLayout(this);
    layout->setRowWrapPolicy(QFormLayout::WrapAllRows);
}

QJsonSchemaObject::QJsonSchemaObject(const QJsonObject &schema, QJsonSchemaWidget *parent) : QJsonSchemaObject(parent)
{
    setSchema(schema);
}

void QJsonSchemaObject::setSchema(const QJsonObject &s)
{
    // QJsonObject j = getParentForm()->dereference(s);
    auto schema = s;

    auto *l = formLayout();

    // if (tabwidget) {
    //     tabwidget->deleteLater();
    //     tabwidget = nullptr;
    // }

    while (l->rowCount()) {
        l->removeRow(l->rowCount() - 1);
    }
    widgetsMap.clear();

    // 间距
    if (schema.contains("ui:spacing")) {
        auto spacing = schema.find("ui:spacing")->toInt();
        l->setVerticalSpacing(spacing);
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
            QJsonSchemaWidgetsFactory factory;
            auto *widget = factory.createWidget(prop, this);
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

            widgetsMap[key] = {title, widget};
        }
    }

    // ui格式
    QString uiWidget = "rows";
    if (schema.contains("ui:widget")) {
        uiWidget = schema.find("ui:widget")->toString();
    }
    // if (uiWidget == "tabs") {
    //     tabwidget = new QTabWidget(this);
    //     l->addRow(tabwidget);
    // }

    // 顺序
    if (schema.contains("ui:order")) {
        auto order = schema.find("ui:order")->toArray();
        for (auto name : order) {
            if (widgetsMap.contains(name.toString())) {
                if (uiWidget == "tabs") {
                    // tabwidget->addTab(props.at(x.toString()).second, props.at(x.toString()).first);
                } else {
                    l->addRow(widgetsMap[name.toString()].first, widgetsMap[name.toString()].second);
                }
            }
            widgetsMap.erase(name.toString());
        }
    } else {
        for (auto &widget : widgetsMap) {
            if (uiWidget == "tabs") {
                // tabwidget->addTab(x.second.second, x.second.first);
            } else {
                l->addRow(widget.second.first, widget.second.second);
            }
        }
    }
}

QJsonValue QJsonSchemaObject::getValue() const
{
    QJsonObject json;
    for (const auto &widget : widgetsMap) {
        json[widget.first] = widget.second.second->getValue();
    }
    return json;
}

void QJsonSchemaObject::setValue(const QJsonObject &json)
{
    auto setV = [](QJsonSchemaWidget *widget, const QJsonValue &value) {
        if (auto *w = dynamic_cast<QJsonSchemaNumber *>(widget)) {
            if (value.isDouble()) {
                w->setValue(value.toDouble());
            }
        } else if (auto *w = dynamic_cast<QJsonSchemaString *>(widget)) {
            if (value.isString()) {
                w->setValue(value.toString());
            }
        } else if (auto *w = dynamic_cast<QJsonSchemaObject *>(widget)) {
            if (value.isObject()) {
                w->setValue(value.toObject());
            }
            // } else if (auto *w = dynamic_cast<QJsonSchemaArray *>(widget)) {
            //     if (value.isArray()) {
            //         w->setValue(value.toArray());
            //     }
        } else if (auto *w = dynamic_cast<QJsonSchemaBoolean *>(widget)) {
            if (value.isBool()) {
                w->setValue(value.toBool());
            }
        }
    };

    for (const auto &key : json.keys()) {
        if (widgetsMap.contains(key)) {
            // widgets.at(key).second->setValue(json[key]);

            setV(widgetsMap[key].second, json[key]);
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

// #pragma region /*array*/

// QJsonSchemaArray::QJsonSchemaArray(QJsonSchemaWidget *parent) : QJsonSchemaWidget(parent)
// {
//     // QFormLayout * h  = new QFormLayout(this);
//     // h->setMargin(3);
//     // this->setLayout(h);

//     {
//         auto *l = new QFormLayout(this);

//         propertiesLayout = new QFormLayout();

//         {
//             oneOf = new QComboBox(this);
//         }

//         auto *h = new QHBoxLayout();
//         h->setAlignment(Qt::AlignRight);
//         add = new QToolButton(this);
//         add->setText("+");

//         connect(add, &QToolButton::released, [this]() {
//             auto i = oneOf->currentIndex();
//             pushBack(oneOfArray.at(i).toObject());
//             rebuild();
//         });

//         h->addWidget(oneOf);
//         h->addWidget(add);

//         // _propertiesLayout->setMargin(3);

//         l->addRow(propertiesLayout);
//         l->addRow(h);

//         this->setLayout(l);

//         {
//             listWidget = new QListWidget(this);
//             l->addRow(listWidget);
//         }
//     }
// }

// QJsonSchemaArray::QJsonSchemaArray(const QJsonObject &schema, QJsonSchemaWidget *parent) : QJsonSchemaArray(parent)
// {
//     setSchema(schema);
// }

// void QJsonSchemaArray::pushBack(QJsonObject o)
// {
//     auto *w = new QJValue(this, parentForm);
//     auto *h = new QHBoxLayout();

//     w->setSchema(o);
//     h->addWidget(w);

//     auto *up = new QToolButton(this);
//     up->setArrowType(Qt::UpArrow);
//     auto *down = new QToolButton(this);
//     down->setArrowType(Qt::DownArrow);
//     auto *del = new QToolButton(this);
//     del->setText("✖");

//     up->setVisible(!fixedOrder);
//     down->setVisible(!fixedOrder);
//     // del->setVisible(showButtons);

//     up->setMaximumSize(25, 25);
//     down->setMaximumSize(25, 25);
//     del->setMaximumSize(25, 25);

//     connect(down, &QAbstractButton::clicked, [w, this](bool) {
//         size_t ro = 0;
//         for (auto &x : items) {
//             if (x.widget == w) {
//                 if (ro != items.size() - 1) {
//                     std::swap(items[ro], items[ro + 1]);
//                     rebuild();
//                     Q_EMIT changed();
//                     return;
//                 }
//             }
//             ro++;
//         }
//     });

//     connect(up, &QAbstractButton::clicked, [w, this](bool) {
//         size_t ro = 0;
//         for (auto &x : items) {
//             if (x.widget == w) {
//                 if (ro != 0) {
//                     std::swap(items[ro - 1], items[ro]);
//                     rebuild();
//                     Q_EMIT changed();
//                     return;
//                 }
//             }
//             ro++;
//         }
//     });

//     connect(del, &QAbstractButton::clicked, [w, this](bool) {
//         int ro = 0;
//         for (auto &x : items) {
//             if (x.widget == w) {
//                 delete x.widget;
//                 delete x.down;
//                 delete x.up;
//                 delete x.del;
//                 delete x.layout;
//                 items.erase(items.begin() + ro);
//                 Q_EMIT changed();
//                 return;
//             }
//             ro++;
//         }
//     });

//     h->addWidget(up);
//     h->addWidget(down);
//     h->addWidget(del);

//     items.push_back({w, h, up, down, del});
// }

// void QJsonSchemaArray::rebuild()
// {
//     auto *layout = propertiesLayout;

//     while (layout->rowCount()) {
//         auto h = layout->takeRow(layout->rowCount() - 1);
//         bool found = false;
//         for (auto &x : items) {
//             if (h.fieldItem == x.layout) {
//                 found = true;
//             }
//         }
//         if (!found) {
//             delete h.fieldItem;
//         }
//     }

//     for (auto &x : items) {
//         x.del->setVisible(!fixedSize);
//         layout->addRow(x.layout);
//     }
// }

// void QJsonSchemaArray::setSchema(const QJsonObject &s)
// {
//     // QJsonObject j = getParentForm()->dereference(s);
//     auto schema = s;

//     oneOf->setVisible(false);
//     add->setVisible(false);
//     fixedSize = true;

//     listWidget->clear();
//     listWidget->hide();

//     if (schema.contains("ui:spacing")) {
//         auto spacing = schema.find("ui:spacing")->toInt();
//         propertiesLayout->setVerticalSpacing(spacing);
//         listWidget->setSpacing(spacing);
//     }
//     if (schema.contains("ui:fixedOrder")) {
//         fixedOrder = schema.find("ui:fixedOrder")->toBool();
//     }

//     if (schema.contains("additionalItems")) {
//         auto aItems = schema.find("additionalItems");
//         if (aItems->isArray()) {
//             oneOfArray = aItems->toArray();
//         } else if (aItems->isObject()) {
//             oneOfArray.push_back(aItems->toObject());
//         }
//         for (int i = 0; i < oneOfArray.size(); i++) {
//             QString title = "ite_" + QString::number(i);
//             auto obj = oneOfArray.at(i).toObject();
//             if (obj.contains("title")) {
//                 title = obj.find("title")->toString();
//             }
//             oneOf->addItem(title);
//         }

//         if (oneOfArray.size() >= 2) {
//             oneOf->setVisible(true);
//         }
//         add->setVisible(true);
//         fixedSize = false;
//     }

//     bool uniqueItems = false;
//     if (schema.contains("uniqueItems")) {
//         uniqueItems = schema.find("uniqueItems")->toBool();
//     }

//     if (schema.contains("items")) {
//         auto it = schema.find("items");

//         if (it->isArray()) {
//             auto p = it->toArray();

//             int ro = 0;
//             for (auto i : p) {
//                 pushBack(i.toObject());

//                 ro++;
//             }
//         } else if (it->isObject()) {
//             auto p = it->toObject();

//             if (p.find("type")->toString() == "string" && p.contains("enum") && uniqueItems) {
//                 int i = 0;
//                 for (auto v : p.find("enum")->toArray()) {
//                     listWidget->insertItem(i++, v.toString());
//                 }
//                 listWidget->setSelectionMode(QAbstractItemView::SelectionMode::MultiSelection);
//                 listWidget->show();
//             }
//         }
//     }
//     rebuild();
// }

// QJsonValue QJsonSchemaArray::getValue() const
// {
//     QJsonArray o;
//     if (listWidget->count() == 0) {
//         for (const auto &x : items) {
//             o.push_back(x.widget->getValue());
//         }
//     } else {
//         for (auto *x : listWidget->selectedItems()) {
//             o.push_back(x->text());
//         }
//     }
//     return o;
// }

// void QJsonSchemaArray::setValue(QJsonArray data)
// {
//     auto mm = std::min(static_cast<size_t>(data.size()), items.size());
//     for (size_t i = 0; i < mm; i++) {
//         items[i].widget->setValue(data[i]);
//     }
// }

// #pragma endregion /*array*/

#pragma region /*string*/

QJsonSchemaString::QJsonSchemaString(QJsonSchemaWidget *parent) : QJsonSchemaWidget(parent)
{
    auto *h = new QHBoxLayout(this);

    widget = new QLineEdit(this);
    combo = new QComboBox(this);
    fileButton = new QPushButton(this);
    dirButton = new QPushButton(this);
    colorButton = new QPushButton(this);
    dateEdit = new QDateEdit(this);

    h->addWidget(widget, 0);
    h->addWidget(fileButton, 0);
    h->addWidget(dirButton, 0);
    h->addWidget(colorButton, 0);
    h->addWidget(combo, 0);
    h->addWidget(dateEdit, 0);

    connect(widget, &QLineEdit::textChanged, [this]() { Q_EMIT changed(); });

    connect(dateEdit, &QDateEdit::dateChanged, [this](const QDate &date) {
        // auto fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),
        // "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
        widget->setText(date.toString(Qt::ISODate));
        // _fileButton->setText(fileName);
    });

    connect(fileButton, &QPushButton::clicked, [this](bool) {
        // auto fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),
        // "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
        auto fileName = QFileDialog::getOpenFileName(this);
        widget->setText(fileName);
        // _fileButton->setText(fileName);
    });

    connect(dirButton, &QPushButton::clicked, [this](bool) {
        // auto fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),
        // "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
        auto fileName = QFileDialog::getExistingDirectory(this);
        widget->setText(fileName);
        // _fileButton->setText(fileName);
    });

    connect(colorButton, &QPushButton::clicked, [this](bool) {
        // auto fileName = QFileDialog::getOpenFileName(this, tr("Choose File"),
        // "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
        // auto fileName = QFileDialog::getExistingDirectory(this);
        auto c = QColorDialog::getColor(Qt::white, this, "Choose a Color",
                                        QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
        int r, g, b, a;
        c.getRgb(&r, &g, &b, &a);
        char txt[25];
        int cx = std::snprintf(txt, 25, "#%02x%02x%02x%02x", r, g, b, a);
        widget->setText(txt);
    });

    connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int) { widget->setText(combo->currentText()); });
}

QJsonSchemaString::QJsonSchemaString(const QJsonObject &schema, QJsonSchemaWidget *parent) : QJsonSchemaString(parent)
{
    setSchema(schema);
}

void QJsonSchemaString::setSchema(const QJsonObject &s)
{
    // QJsonObject j = getParentForm()->dereference(s);
    auto schema = s;

    QString wid = "";

    QString defaultValue;
    {
        auto mI = schema.find("default");
        if (mI != schema.end()) {
            defaultValue = mI->toString();
        }
    }

    {
        auto mI = schema.find("ui:widget");
        if (mI != schema.end()) {
            wid = mI->toString();
        }
    }

    {
        auto mI = schema.find("enum");
        if (mI != schema.end()) {
            auto e = mI->toArray();

            for (auto en : e) {
                if (en.isString()) {
                    combo->addItem(en.toString());
                }
            }

            fileButton->setVisible(false);
            dirButton->setVisible(false);
            widget->setVisible(false);
            colorButton->setVisible(false);
            dateEdit->setVisible(false);
            combo->setVisible(true);
            setValue(defaultValue);
            return;
        }
    }

    // widget->setText(defaultValue);
    if (wid == "file") {
        fileButton->setVisible(true);
        dirButton->setVisible(false);
        combo->setVisible(false);
        colorButton->setVisible(false);
        dateEdit->setVisible(false);
    } else if (wid == "date") {
        dirButton->setVisible(false);
        fileButton->setVisible(false);
        combo->setVisible(false);
        colorButton->setVisible(false);
        widget->setVisible(false);
        dateEdit->setVisible(true);
    } else if (wid == "dir") {
        fileButton->setVisible(false);
        dirButton->setVisible(true);
        combo->setVisible(false);
        colorButton->setVisible(false);
        dateEdit->setVisible(false);
    } else if (wid == "color" || wid == "colour") {
        dirButton->setVisible(false);
        fileButton->setVisible(false);
        combo->setVisible(false);
        colorButton->setVisible(true);
        dateEdit->setVisible(false);
    } else {
        dirButton->setVisible(false);
        fileButton->setVisible(false);
        combo->setVisible(false);
        colorButton->setVisible(false);
        dateEdit->setVisible(false);
    }

    setValue(defaultValue);
}

QJsonValue QJsonSchemaString::getValue() const
{
    if (auto *w = dynamic_cast<QLineEdit *>(widget)) {
        return w->text();
    }
    return {};
}

void QJsonSchemaString::setValue(QString s)
{
    if (combo) {
        for (int i = 0; i < combo->count(); i++) {
            if (combo->itemText(i) == s) {
                combo->setCurrentIndex(i);
                break;
            }
        }
    }
    if (dateEdit->isVisible()) {
        QDate d;
        d = QDate::fromString(s, Qt::ISODate);
        dateEdit->setDate(d);
    } else {
        if (widget) {
            widget->setText(s);
        }
    }
}

#pragma endregion /*string*/

#pragma region /*boolean*/

QJsonSchemaBoolean::QJsonSchemaBoolean(QJsonSchemaWidget *parent) : QJsonSchemaWidget(parent)
{
    auto *h = new QHBoxLayout(this);
    //   h->setMargin(0);

    auto *le = new QCheckBox(this);
    auto *s = new ToggleSwitch(8, 10, this);

    le->setLayoutDirection(Qt::RightToLeft);
    _switch = s;
    connect(_switch, &QAbstractButton::clicked, [this](bool ch) { _widget->setChecked(ch); });

    _widget = le;

    connect(_widget, &QAbstractButton::toggled, [this]() { Q_EMIT changed(); });

    h->setAlignment(Qt::AlignLeft);

    h->addWidget(new QWidget(), 1);
    h->addWidget(le, 1);
    h->addWidget(s, 1);
    h->update();
}

QJsonSchemaBoolean::QJsonSchemaBoolean(const QJsonObject &schema, QJsonSchemaWidget *parent)
    : QJsonSchemaBoolean(parent)
{
    setSchema(schema);
}

void QJsonSchemaBoolean::setSchema(const QJsonObject &s)
{
    // QJsonObject j = getParentForm()->dereference(s);
    auto schema = s;

    {
        auto mI = schema.find("default");
        if (mI != schema.end()) {
            auto def = mI->toBool(false);
            _widget->setChecked(def);
            _switch->setChecked(def);
        }
    }
    QString wid = "checkbox";

    {
        auto mI = schema.find("ui:widget");
        if (mI != schema.end()) {
            wid = mI->toString("checkbox");
        }
    }

    if (wid == "switch") {
        _switch->setVisible(true);
        _widget->setVisible(false);
    } else {
        _switch->setVisible(false);
        _widget->setVisible(true);
    }
}

QJsonValue QJsonSchemaBoolean::getValue() const
{
    return _widget->isChecked();
}

void QJsonSchemaBoolean::setValue(bool b)
{
    if (_switch) {
        _switch->setChecked(b);
    }
    if (_widget) {
        _widget->setChecked(b);
    }
}

#pragma endregion /*boolean*/

#pragma region /*number*/

QJsonSchemaNumber::QJsonSchemaNumber(QJsonSchemaWidget *parent) : QJsonSchemaWidget(parent) {}

QJsonSchemaNumber::QJsonSchemaNumber(const QJsonObject &schema, QJsonSchemaWidget *parent) : QJsonSchemaNumber(parent)
{
    setSchema(schema);
}

void QJsonSchemaNumber::setSchema(const QJsonObject &s)
{
    // auto schema = getParentForm()->dereference(s);
    auto schema = s;

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
            combo->setObjectName("combo");

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
        auto *h = new QHBoxLayout(sliderBox);
        auto *slider = new QSlider(Qt::Horizontal, sliderBox);
        slider->setObjectName("slider");
        auto *spinBox = new QDoubleSpinBox(sliderBox);
        spinBox->setObjectName("spinBox");
        h->addWidget(slider, 10);
        h->addWidget(spinBox, 1);

        // spinBox 值改变时,更新
        connect(spinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double) { Q_EMIT changed(); });

        // silder 改变时，spinBox 变化
        connect(slider, &QSlider::valueChanged,
                // widget,
                [slider, spinBox](int i) {
                    double t = static_cast<double>(i - slider->minimum()) /
                               static_cast<double>(slider->maximum() - slider->minimum());
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
    // return dynamic_cast<QDoubleSpinBox *>(_widget)->value();

    return {};
}

void QJsonSchemaNumber::setValue(double b)
{
    // _widget->setValue(b);
    // double t =
    //     static_cast<double>(b - _widget->minimum()) / static_cast<double>(_widget->maximum() -
    //     _widget->minimum());
    // double r = _slider->minimum() * (1.0 - t) + _slider->maximum() * t;
    // _slider->blockSignals(true);
    // _slider->setValue(static_cast<int>(r));
    // _slider->blockSignals(false);
}

#pragma endregion /*number*/

}  // namespace QJsonSchemaForm
QT_END_NAMESPACE