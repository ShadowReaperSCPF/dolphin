/***************************************************************************
 *   Copyright (C) 2011 by Peter Penz <peter.penz19@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#include "kfileitemclipboard.h"

#include <KUrlMimeData>

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

class KFileItemClipboardSingleton
{
public:
    KFileItemClipboard instance;
};
Q_GLOBAL_STATIC(KFileItemClipboardSingleton, s_KFileItemClipboard)



KFileItemClipboard* KFileItemClipboard::instance()
{
    return &s_KFileItemClipboard->instance;
}

bool KFileItemClipboard::isCut(const QUrl& url) const
{
    return m_cutItems.contains(url);
}

QList<QUrl> KFileItemClipboard::cutItems() const
{
    return m_cutItems.values();
}

KFileItemClipboard::~KFileItemClipboard()
{
}

void KFileItemClipboard::updateCutItems()
{
    const QMimeData* mimeData = QApplication::clipboard()->mimeData();

    // mimeData can be 0 according to https://bugs.kde.org/show_bug.cgi?id=335053
    if (!mimeData) {
        m_cutItems.clear();
        emit cutItemsChanged();
        return;
    }

    const QByteArray data = mimeData->data(QStringLiteral("application/x-kde-cutselection"));
    const bool isCutSelection = (!data.isEmpty() && data.at(0) == QLatin1Char('1'));
    if (isCutSelection) {
        m_cutItems = KUrlMimeData::urlsFromMimeData(mimeData).toSet();
    } else {
        m_cutItems.clear();
    }
    emit cutItemsChanged();
}

KFileItemClipboard::KFileItemClipboard() :
    QObject(nullptr),
    m_cutItems()
{
    updateCutItems();

    connect(QApplication::clipboard(), &QClipboard::dataChanged,
            this, &KFileItemClipboard::updateCutItems);
}
