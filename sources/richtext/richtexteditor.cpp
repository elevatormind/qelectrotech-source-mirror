/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/**
	Integration : QElectroTech Team
	Changelog:
	- 09/04/2013 : Start integration...Compilation and object creation are successful
*/

#include "richtexteditor_p.h"
#include "ui_addlinkdialog.h"

//#include <QtDesigner/QDesignerFormEditorInterface>

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QPointer>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QXmlStreamAttributes>

#include <QAction>
#include <QColorDialog>
#include <QComboBox>
#include <QtGui/QFontDatabase>
#include <QtGui/QTextCursor>
#include <QtGui/QPainter>
#include <QtGui/QIcon>
#include <QMenu>
#include <QtGui/QMoveEvent>
#include <QTabWidget>
#include <QtGui/QTextDocument>
#include <QtGui/QTextBlock>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QActionGroup>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
#include <QStringView>
#else
#define QStringView QStringRef
#endif

QT_BEGIN_NAMESPACE

//static const char RichTextDialogGroupC[] = "RichTextDialog";
//static const char GeometryKeyC[] = "Geometry";
//static const char TabKeyC[] = "Tab";

//const bool simplifyRichTextDefault = true;

namespace qdesigner_internal {
	// Richtext simplification filter helpers: Elements to be discarded
	static inline bool filterElement(const QStringView &name)
	{
		return name != QLatin1String("meta") && name != QLatin1String("style");
	}

	// Richtext simplification filter helpers: Filter attributes of elements
	static inline void filterAttributes(
			const QStringView &name,
			QXmlStreamAttributes *atts,
			bool *paragraphAlignmentFound)
	{
		typedef QXmlStreamAttributes::iterator AttributeIt;

		if (atts->isEmpty())
			return;

		 // No style attributes for <body>
		if (name == QLatin1String("body")) {
			atts->clear();
			return;
		}

		// Clean out everything except 'align' for 'p'
		if (name == QLatin1String("p")) {
			for (AttributeIt it = atts->begin(); it != atts->end(); ) {
				if (it->name() == QLatin1String("align")) {
					++it;
					*paragraphAlignmentFound = true;
				} else {
					it = atts->erase(it);
				}
			}
			return;
		}
	}

	// Richtext simplification filter helpers: Check for blank QStringView.
	static inline bool isWhiteSpace(const QStringView &in)
	{
		const int count = in.size();
		for (int i = 0; i < count; i++)
			if (!in.at(i).isSpace())
				return false;
		return true;
	}

	// Richtext simplification filter: Remove hard-coded font settings,
	// <style> elements, <p> attributes other than 'align' and
	// and unnecessary meta-information.
	QString simplifyRichTextFilter(const QString &in, bool *isPlainTextPtr = nullptr)
	{
		unsigned elementCount = 0;
		bool paragraphAlignmentFound = false;
		QString out;
		QXmlStreamReader reader(in);
		QXmlStreamWriter writer(&out);
		writer.setAutoFormatting(false);
		writer.setAutoFormattingIndent(0);

		while (!reader.atEnd()) {
			switch (reader.readNext()) {
			case QXmlStreamReader::StartElement:
				elementCount++;
				if (filterElement(reader.name())) {
					const QStringView name = reader.name();
					QXmlStreamAttributes attributes = reader.attributes();
					filterAttributes(name, &attributes, &paragraphAlignmentFound);
					writer.writeStartElement(name.toString());
					if (!attributes.isEmpty())
						writer.writeAttributes(attributes);
				} else {
					reader.readElementText(); // Skip away all nested elements and characters.
				}
				break;
			case QXmlStreamReader::Characters:
				if (!isWhiteSpace(reader.text()))
					writer.writeCharacters(reader.text().toString());
				break;
			case QXmlStreamReader::EndElement:
				writer.writeEndElement();
				break;
			default:
				break;
			}
		}
		// Check for plain text (no spans, just <html><head><body><p>)
		if (isPlainTextPtr)
			*isPlainTextPtr = !paragraphAlignmentFound && elementCount == 4u;
		return out;
	}

class RichTextEditor : public QTextEdit
{
	Q_OBJECT
	public:
	RichTextEditor(QWidget *parent = nullptr);
	void setDefaultFont(QFont font);

	QToolBar *createToolBar(QWidget *parent = nullptr);
	bool simplifyRichText() const      { return m_simplifyRichText; }

	public slots:
	void setFontBold(bool b);
	void setFontPointSize(double);
	void setText(const QString &text);
	void setSimplifyRichText(bool v);
	QString text(Qt::TextFormat format) const;

	signals:
	void stateChanged();
	void simplifyRichTextChanged(bool);

	private:
	bool m_simplifyRichText;
};

class AddLinkDialog : public QDialog
{
	Q_OBJECT

	public:
	AddLinkDialog(RichTextEditor *editor, QWidget *parent = nullptr);
	~AddLinkDialog() override;

	int showDialog();

	public slots:
	void accept() override;

	private:
	RichTextEditor *m_editor;
	Ui::AddLinkDialog *m_ui;
};

AddLinkDialog::AddLinkDialog(RichTextEditor *editor, QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::AddLinkDialog)
{
	m_ui->setupUi(this);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_editor = editor;
}

AddLinkDialog::~AddLinkDialog()
{
	delete m_ui;
}

int AddLinkDialog::showDialog()
{
	// Set initial focus
	const QTextCursor cursor = m_editor->textCursor();
	if (cursor.hasSelection()) {
		m_ui->titleInput->setText(cursor.selectedText());
		m_ui->urlInput->setFocus();
	} else {
		m_ui->titleInput->setFocus();
	}

	return exec();
}

void AddLinkDialog::accept()
{
	const QString title = m_ui->titleInput->text();
	const QString url = m_ui->urlInput->text();

	if (!title.isEmpty()) {
		QString html = QLatin1String("<a href=\"");
		html += url;
		html += QLatin1String("\">");
		html += title;
		html += QLatin1String("</a>");

		m_editor->insertHtml(html);
	}

	m_ui->titleInput->clear();
	m_ui->urlInput->clear();

	QDialog::accept();
}

class HtmlTextEdit : public QTextEdit
{
	Q_OBJECT

	public:
	HtmlTextEdit(QWidget *parent = nullptr)
		: QTextEdit(parent)
	{}

	void contextMenuEvent(QContextMenuEvent *event) override;

	private slots:
	void actionTriggered(QAction *action);
};

void HtmlTextEdit::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = createStandardContextMenu();
	QMenu *htmlMenu = new QMenu(tr("Insert HTML entity"), menu);

	typedef struct {
		const char *text;
		const char *entity;
	} Entry;

	const Entry entries[] = {
		{ "&&amp; (&&)", "&amp;" },
		{ "&&nbsp;", "&nbsp;" },
		{ "&&lt; (<)", "&lt;" },
		{ "&&gt; (>)", "&gt;" },
		{ "&&copy; (Copyright)", "&copy;" },
		{ "&&reg; (Trade Mark)", "&reg;" },
	};

	for (int i = 0; i < 6; ++i) {
		QAction *entityAction = new QAction(QLatin1String(entries[i].text),
											htmlMenu);
		entityAction->setData(QLatin1String(entries[i].entity));
		htmlMenu->addAction(entityAction);
	}

	menu->addMenu(htmlMenu);
	connect(htmlMenu, SIGNAL(triggered(QAction*)),
			SLOT(actionTriggered(QAction*)));
	menu->exec(event->globalPos());
	delete menu;
}

void HtmlTextEdit::actionTriggered(QAction *action)
{
	insertPlainText(action->data().toString());
}

class ColorAction : public QAction
{
	Q_OBJECT

	public:
	ColorAction(QObject *parent);

	const QColor& color() const { return m_color; }
	void setColor(const QColor &color);

	signals:
	void colorChanged(const QColor &color);

	private slots:
	void chooseColor();

	private:
	QColor m_color;
};

ColorAction::ColorAction(QObject *parent):
	QAction(parent)
{
	setText(tr("Text Color"));
	setColor(Qt::black);
	connect(this, SIGNAL(triggered()), this, SLOT(chooseColor()));
}

void ColorAction::setColor(const QColor &color)
{
	if (color == m_color)
		return;
	m_color = color;
	QPixmap pix(24, 24);
	QPainter painter(&pix);
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.fillRect(pix.rect(), m_color);
	painter.setPen(m_color.darker());
	painter.drawRect(pix.rect().adjusted(0, 0, -1, -1));
	setIcon(pix);
}

void ColorAction::chooseColor()
{
	const QColor col = QColorDialog::getColor(m_color, nullptr);
	if (col.isValid() && col != m_color) {
		setColor(col);
		emit colorChanged(m_color);
	}
}

class RichTextEditorToolBar : public QToolBar
{
	Q_OBJECT
	public:
	RichTextEditorToolBar(RichTextEditor *editor,
						  QWidget *parent = nullptr);

	public slots:
	void updateActions();

	private slots:
	void alignmentActionTriggered(QAction *action);
	void sizeInputActivated(const QString &size);
	void colorChanged(const QColor &color);
	void setVAlignSuper(bool super);
	void setVAlignSub(bool sub);
	void insertLink();
	void insertImage();


	private:
	QAction *m_bold_action;
	QAction *m_italic_action;
	QAction *m_underline_action;
	QAction *m_valign_sup_action;
	QAction *m_valign_sub_action;
	QAction *m_align_left_action;
	QAction *m_align_center_action;
	QAction *m_align_right_action;
	QAction *m_align_justify_action;
	QAction *m_link_action;
	QAction *m_image_action;
	QAction *m_simplify_richtext_action;
	ColorAction *m_color_action;
	QComboBox *m_font_size_input;

	QPointer<RichTextEditor> m_editor;
};

static QAction *createCheckableAction(const QIcon &icon, const QString &text,
									  QObject *receiver, const char *slot,
									  QObject *parent = nullptr)
{
	QAction *result = new QAction(parent);
	result->setIcon(icon);
	result->setText(text);
	result->setCheckable(true);
	result->setChecked(false);
	if (slot)
		QObject::connect(result, SIGNAL(triggered(bool)), receiver, slot);
	return result;
}

RichTextEditorToolBar::RichTextEditorToolBar(RichTextEditor *editor,
											 QWidget *parent) :
	QToolBar(parent),
	m_link_action(new QAction(this)),
	m_image_action(new QAction(this)),
	m_color_action(new ColorAction(this)),
	m_font_size_input(new QComboBox),
	m_editor(editor)
{

	// Font size combo box
	m_font_size_input->setEditable(false);
	const QList<int> font_sizes = QFontDatabase::standardSizes();
	foreach (int font_size, font_sizes)
		m_font_size_input->addItem(QString::number(font_size));

	connect(m_font_size_input, SIGNAL(activated(QString)),
			this, SLOT(sizeInputActivated(QString)));
	addWidget(m_font_size_input);


	// Bold, italic and underline buttons

	m_bold_action = createCheckableAction(
				QIcon(":/ico/32x32/format-text-bold.png"),
				tr("Texte en gras"), editor, SLOT(setFontBold(bool)), this);
	m_bold_action->setShortcut(Qt::CTRL | Qt::Key_B);
	addAction(m_bold_action);

	m_italic_action = createCheckableAction(
				QIcon(":/ico/32x32/format-text-italic.png"),
				tr("Texte en italique"), editor, SLOT(setFontItalic(bool)), this);
	m_italic_action->setShortcut(Qt::CTRL | Qt::Key_I);
	addAction(m_italic_action);

	m_underline_action = createCheckableAction(
				QIcon(":/ico/32x32/format-text-underline.png"),
				tr("Texte souligé"), editor, SLOT(setFontUnderline(bool)), this);
	m_underline_action->setShortcut(Qt::CTRL | Qt::Key_U);
	addAction(m_underline_action);


	// Left, center, right and justified alignment buttons

	QActionGroup *alignment_group = new QActionGroup(this);
	connect(alignment_group, SIGNAL(triggered(QAction*)),
			SLOT(alignmentActionTriggered(QAction*)));

	m_align_left_action = createCheckableAction(
				QIcon(),
				tr("Left Align"), editor, nullptr, alignment_group);
	addAction(m_align_left_action);

	m_align_center_action = createCheckableAction(
				QIcon(),
				tr("Center"), editor, nullptr, alignment_group);
	addAction(m_align_center_action);

	m_align_right_action = createCheckableAction(
				QIcon(),
				tr("Right Align"), editor, nullptr, alignment_group);
	addAction(m_align_right_action);

	m_align_justify_action = createCheckableAction(
				QIcon(),
				tr("Justify"), editor, nullptr, alignment_group);
	addAction(m_align_justify_action);

	m_align_justify_action -> setVisible( false );
	m_align_center_action -> setVisible( false );
	m_align_left_action -> setVisible( false );
	m_align_right_action -> setVisible( false );

	// Superscript and subscript buttons

	m_valign_sup_action = createCheckableAction(
				QIcon(":/ico/22x22/format-text-superscript.png"),
				tr("Superscript"),
				this, SLOT(setVAlignSuper(bool)), this);
	addAction(m_valign_sup_action);

	m_valign_sub_action = createCheckableAction(
				QIcon(":/ico/22x22/format-text-subscript.png"),
				tr("Subscript"),
				this, SLOT(setVAlignSub(bool)), this);
	addAction(m_valign_sub_action);

	m_valign_sup_action -> setVisible( true );
	m_valign_sub_action -> setVisible( true );

	// Insert hyperlink and image buttons

	m_link_action->setText(tr("Insérer un lien"));
	connect(m_link_action, SIGNAL(triggered()), SLOT(insertLink()));
	addAction(m_link_action);

	m_image_action->setText(tr("Insert &Image"));
	connect(m_image_action, SIGNAL(triggered()), SLOT(insertImage()));
	addAction(m_image_action);

	m_image_action -> setVisible( false );
	addSeparator();

	// Text color button
	connect(m_color_action, SIGNAL(colorChanged(QColor)),
			this, SLOT(colorChanged(QColor)));
	addAction(m_color_action);


	// Simplify rich text
	m_simplify_richtext_action = createCheckableAction(
				QIcon(":/ico/32x32/simplifyrichtext.png"),
				tr("Simplify Rich Text"), editor, SLOT(setSimplifyRichText(bool)),this);
	m_simplify_richtext_action->setChecked(editor->simplifyRichText());
	connect(m_editor, SIGNAL(simplifyRichTextChanged(bool)),
			m_simplify_richtext_action, SLOT(setChecked(bool)));
	addAction(m_simplify_richtext_action);

	connect(editor, SIGNAL(textChanged()), this, SLOT(updateActions()));
	connect(editor, SIGNAL(stateChanged()), this, SLOT(updateActions()));

	updateActions();
}

void RichTextEditorToolBar::alignmentActionTriggered(QAction *action)
{
	Qt::Alignment new_alignment;

	if (action == m_align_left_action) {
		new_alignment = Qt::AlignLeft;
	} else if (action == m_align_center_action) {
		new_alignment = Qt::AlignCenter;
	} else if (action == m_align_right_action) {
		new_alignment = Qt::AlignRight;
	} else {
		new_alignment = Qt::AlignJustify;
	}

	m_editor->setAlignment(new_alignment);
}



void RichTextEditorToolBar::colorChanged(const QColor &color)
{
	m_editor->setTextColor(color);
	m_editor->setFocus();
}

void RichTextEditorToolBar::sizeInputActivated(const QString &size)
{
	bool ok;
	int i = size.toInt(&ok);
	if (!ok)
		return;

	m_editor->setFontPointSize(i);
	m_editor->setFocus();
}

void RichTextEditorToolBar::setVAlignSuper(bool super)
{
	const QTextCharFormat::VerticalAlignment align = super ?
				QTextCharFormat::AlignSuperScript : QTextCharFormat::AlignNormal;

	QTextCharFormat charFormat = m_editor->currentCharFormat();
	charFormat.setVerticalAlignment(align);
	m_editor->setCurrentCharFormat(charFormat);

	m_valign_sub_action->setChecked(false);
}

void RichTextEditorToolBar::setVAlignSub(bool sub)
{
	const QTextCharFormat::VerticalAlignment align = sub ?
				QTextCharFormat::AlignSubScript : QTextCharFormat::AlignNormal;

	QTextCharFormat charFormat = m_editor->currentCharFormat();
	charFormat.setVerticalAlignment(align);
	m_editor->setCurrentCharFormat(charFormat);

	m_valign_sup_action->setChecked(false);
}

void RichTextEditorToolBar::insertLink()
{
	AddLinkDialog linkDialog(m_editor, this);
	linkDialog.showDialog();
	m_editor->setFocus();
}

void RichTextEditorToolBar::insertImage()
{
#ifdef hip
	const QString path = IconSelector::choosePixmapResource(m_core, m_core->resourceModel(), QString(), this);
	if (!path.isEmpty())
		m_editor->insertHtml(QLatin1String("<img src=\"") + path + QLatin1String("\"/>"));
#endif
}

void RichTextEditorToolBar::updateActions()
{
	if (m_editor == nullptr) {
		setEnabled(false);
		return;
	}

	const Qt::Alignment alignment = m_editor->alignment();
	const QTextCursor cursor = m_editor->textCursor();
	const QTextCharFormat charFormat = cursor.charFormat();
	const QFont font = charFormat.font();
	const QTextCharFormat::VerticalAlignment valign =
			charFormat.verticalAlignment();
	const bool superScript = valign == QTextCharFormat::AlignSuperScript;
	const bool subScript = valign == QTextCharFormat::AlignSubScript;

	if (alignment & Qt::AlignLeft) {
		m_align_left_action->setChecked(true);
	} else if (alignment & Qt::AlignRight) {
		m_align_right_action->setChecked(true);
	} else if (alignment & Qt::AlignHCenter) {
		m_align_center_action->setChecked(true);
	} else {
		m_align_justify_action->setChecked(true);
	}

	m_bold_action->setChecked(font.bold());
	m_italic_action->setChecked(font.italic());
	m_underline_action->setChecked(font.underline());
	m_valign_sup_action->setChecked(superScript);
	m_valign_sub_action->setChecked(subScript);

	const int size = font.pointSize();
	const int idx = m_font_size_input->findText(QString::number(size));
	if (idx != -1)
		m_font_size_input->setCurrentIndex(idx);

	m_color_action->setColor(m_editor->textColor());
}

RichTextEditor::RichTextEditor(QWidget *parent)
	: QTextEdit(parent)
{
	connect(this, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
			this, SIGNAL(stateChanged()));
	connect(this, SIGNAL(cursorPositionChanged()),
			this, SIGNAL(stateChanged()));
}

QToolBar *RichTextEditor::createToolBar(QWidget *parent)
{
	return new RichTextEditorToolBar(this, parent);
}

void RichTextEditor::setFontBold(bool b)
{
	if (b)
		setFontWeight(QFont::Bold);
	else
		setFontWeight(QFont::Normal);
}

void RichTextEditor::setFontPointSize(double d)
{
	QTextEdit::setFontPointSize(qreal(d));
}

void RichTextEditor::setText(const QString &text)
{
	if (Qt::mightBeRichText(text))
		setHtml(text);
	else
		setPlainText(text);
}

void RichTextEditor::setSimplifyRichText(bool v)
{
	if (v != m_simplifyRichText) {
		m_simplifyRichText = v;
		emit simplifyRichTextChanged(v);
	}
}

void RichTextEditor::setDefaultFont(QFont font)
{
	// Some default fonts on Windows have a default size of 7.8,
	// which results in complicated rich text generated by toHtml().
	// Use an integer value.
	const int pointSize = qRound(font.pointSizeF());
	if (pointSize > 0 && !qFuzzyCompare(qreal(pointSize), font.pointSizeF())) {
		font.setPointSize(pointSize);
	}

	document()->setDefaultFont(font);
	if (font.pointSize() > 0)
		setFontPointSize(font.pointSize());
	else
		setFontPointSize(QFontInfo(font).pointSize());
	emit textChanged();
}

QString RichTextEditor::text(Qt::TextFormat format) const
{
	switch (format) {
		case Qt::PlainText:
			return toPlainText();
		case Qt::RichText:
			return m_simplifyRichText ? simplifyRichTextFilter(toHtml()) : toHtml();
		case Qt::AutoText:
			break;
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#else
#if TODO_LIST
#pragma message("@TODO remove code for QT 5.14 or later")
#endif
		case Qt::MarkdownText: //This enum value was added in Qt 5.14.
			break;
#endif
		default:
			qInfo("(RichTextEditor::text) no valid switch: %d",format);
			break;
	}
	const QString html = toHtml();
	bool isPlainText;
	const QString simplifiedHtml = simplifyRichTextFilter(html, &isPlainText);
	if (isPlainText)
		return toPlainText();
	return m_simplifyRichText ? simplifiedHtml : html;
}

RichTextEditorDialog::RichTextEditorDialog(QWidget *parent)  :
	QDialog(parent),
	m_editor(new RichTextEditor()),
	m_text_edit(new HtmlTextEdit),
	m_tab_widget(new QTabWidget),
	m_state(Clean)
{
	setWindowTitle(tr("Edit text"));
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	m_text_edit->setAcceptRichText(false);

	connect(m_editor, SIGNAL(textChanged()), this, SLOT(richTextChanged()));
	connect(m_editor, SIGNAL(simplifyRichTextChanged(bool)), this, SLOT(richTextChanged()));
	connect(m_text_edit, SIGNAL(textChanged()), this, SLOT(sourceChanged()));

	// The toolbar needs to be created after the RichTextEditor
	QToolBar *tool_bar = m_editor->createToolBar();
	tool_bar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

	QWidget *rich_edit = new QWidget;
	QVBoxLayout *rich_edit_layout = new QVBoxLayout(rich_edit);
	rich_edit_layout->addWidget(tool_bar);
	rich_edit_layout->addWidget(m_editor);

	QWidget *plain_edit = new QWidget;
	QVBoxLayout *plain_edit_layout = new QVBoxLayout(plain_edit);
	plain_edit_layout->addWidget(m_text_edit);

	m_tab_widget->setTabPosition(QTabWidget::South);
	m_tab_widget->addTab(rich_edit, tr("Rich Text"));
	m_tab_widget->addTab(plain_edit, tr("Source"));
	connect(m_tab_widget, SIGNAL(currentChanged(int)),
			SLOT(tabIndexChanged(int)));

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
	QPushButton *ok_button = buttonBox->button(QDialogButtonBox::Ok);
	ok_button->setText(tr("&OK"));
	ok_button->setDefault(true);
	buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("&Cancel"));
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(on_buttonBox_accepted()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(m_tab_widget);
	layout->addWidget(buttonBox);

	m_editor->setFocus();

}

RichTextEditorDialog::~RichTextEditorDialog()
{
}

/**
	@brief RichTextEditorDialog::on_buttonBox_accepted
*/
void RichTextEditorDialog::on_buttonBox_accepted()
{
	emit applyEditText( text(Qt::RichText) );
	this->close();
}


int RichTextEditorDialog::showDialog()
{
	m_tab_widget->setCurrentIndex(0);
	m_editor->selectAll();
	m_editor->setFocus();

	return exec();
}

void RichTextEditorDialog::setDefaultFont(const QFont &font)
{
	m_editor->setDefaultFont(font);
}

void RichTextEditorDialog::setText(const QString &text)
{
	// Generally simplify rich text unless verbose text is found.
	const bool isSimplifiedRichText = !text.startsWith("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">");
	m_editor->setSimplifyRichText(isSimplifiedRichText);
	m_editor->setText(text);
	m_text_edit->setPlainText(text);
	m_state = Clean;
}

QString RichTextEditorDialog::text(Qt::TextFormat format) const
{
	// In autotext mode, if the user has changed the source, use that
	if (format == Qt::AutoText && (m_state == Clean || m_state == SourceChanged))
		return m_text_edit->toPlainText();
	// If the plain text HTML editor is selected, first copy its contents over
	// to the rich text editor so that it is converted to Qt-HTML or actual
	// plain text.
	if (m_tab_widget->currentIndex() == SourceIndex && m_state == SourceChanged)
		m_editor->setHtml(m_text_edit->toPlainText());
	return m_editor->text(format);
}

void RichTextEditorDialog::tabIndexChanged(int newIndex)
{
	// Anything changed, is there a need for a conversion?
	if (newIndex == SourceIndex && m_state != RichTextChanged)
		return;
	if (newIndex == RichTextIndex && m_state != SourceChanged)
		return;
	const State oldState = m_state;
	// Remember the cursor position, since it is invalidated by setPlainText
	QTextEdit *new_edit = (newIndex == SourceIndex) ? m_text_edit : m_editor;
	const int position = new_edit->textCursor().position();

	if (newIndex == SourceIndex) {
		const QString html = m_editor->text(Qt::RichText);
		m_text_edit->setPlainText(html);

	} else
		m_editor->setHtml(m_text_edit->toPlainText());

	QTextCursor cursor = new_edit->textCursor();
	cursor.movePosition(QTextCursor::End);
	if (cursor.position() > position) {
		cursor.setPosition(position);
	}
	new_edit->setTextCursor(cursor);
	m_state = oldState; // Changed is triggered by setting the text
}

void RichTextEditorDialog::richTextChanged()
{
	m_state = RichTextChanged;
}

void RichTextEditorDialog::sourceChanged()
{
	m_state = SourceChanged;
}

} // namespace qdesigner_internal

QT_END_NAMESPACE

#include "richtexteditor.moc"
