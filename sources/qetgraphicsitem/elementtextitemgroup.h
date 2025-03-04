﻿/*
	Copyright 2006-2025 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ELEMENTTEXTITEMGROUP_H
#define ELEMENTTEXTITEMGROUP_H

#include <QGraphicsItemGroup>
#include <QObject>
#include <QDomElement>

class Element;
class DynamicElementTextItem;
class Diagram;
class CrossRefItem;

/**
	@brief The ElementTextItemGroup class
	This class represent a group of element text
	Texts in the group can be aligned left / center /right
*/
class ElementTextItemGroup : public QObject, public  QGraphicsItemGroup
{
	Q_OBJECT
	
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)
	Q_PROPERTY(qreal rotation READ rotation WRITE setRotation NOTIFY rotationChanged)
	Q_PROPERTY(int verticalAdjustment READ verticalAdjustment WRITE setVerticalAdjustment NOTIFY verticalAdjustmentChanged)
	Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment NOTIFY alignmentChanged)
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(bool holdToBottomPage READ holdToBottomPage WRITE setHoldToBottomPage NOTIFY holdToBottomPageChanged)
	Q_PROPERTY(bool frame READ frame WRITE setFrame NOTIFY frameChanged)
	
	public:
	signals:
		void rotationChanged(qreal);
		void verticalAdjustmentChanged(int);
		void alignmentChanged(Qt::Alignment);
		void nameChanged(QString);
		void holdToBottomPageChanged(bool);
		void xChanged();
		void yChanged();
		void frameChanged(bool frame);
	
	public:
		ElementTextItemGroup(const QString &name, Element *parent);
		~ElementTextItemGroup() override;
		void addToGroup(QGraphicsItem *item);
		void removeFromGroup(QGraphicsItem *item);
		void blockAlignmentUpdate(bool block);
		
		void setAlignment(Qt::Alignment alignement);
		Qt::Alignment alignment() const;
		void updateAlignment();
		int verticalAdjustment() const {return m_vertical_adjustment;}
		void setVerticalAdjustment(int v);
		void setName(QString name);
		QString name() const {return m_name;}
		void setHoldToBottomPage(bool hold);
		bool holdToBottomPage() const {return m_hold_to_bottom_of_page;}
		void setFrame(const bool frame);
		bool frame() const;
		QList<DynamicElementTextItem *> texts() const;
		Diagram *diagram() const;
		Element *parentElement() const;
		
		QDomElement toXml(QDomDocument &dom_document) const;
		void fromXml(QDomElement &dom_element);
		static QString xmlTaggName() {return QString("texts_group");}
		
		void paint(QPainter *painter,
			   const QStyleOptionGraphicsItem *option,
			   QWidget *widget) override;
		QRectF boundingRect() const override;
		void setRotation(qreal angle);
		void setPos(const QPointF &pos);
		void setPos(qreal x, qreal y);
		
	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
		void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
		void mouseReleaseEvent(
				QGraphicsSceneMouseEvent *event) override;
		void mouseDoubleClickEvent(
				QGraphicsSceneMouseEvent *event) override;
		void keyPressEvent(QKeyEvent *event) override;
		void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
		void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
		
	private:
		void updateXref();
		void adjustSlaveXrefPos();
		void autoPos();

	private:
		Qt::Alignment m_alignment = Qt::AlignJustify;
		QString m_name;
		bool m_first_move = true,
		m_hold_to_bottom_of_page = false,
		m_block_alignment_update = false,
		m_frame = false;
		QPointF m_initial_position;
		int m_vertical_adjustment = 0;
		CrossRefItem *m_Xref_item = nullptr;
		Element *m_parent_element = nullptr;
		QList<QMetaObject::Connection> m_update_slave_Xref_connection;
		QGraphicsTextItem *m_slave_Xref_item = nullptr;
		QMetaObject::Connection m_XrefChanged_timer,
		m_linked_changed_timer;
};

#endif // ELEMENTTEXTITEMGROUP_H
