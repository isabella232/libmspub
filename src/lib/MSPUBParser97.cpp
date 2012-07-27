/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* libmspub
 * Version: MPL 1.1 / GPLv2+ / LGPLv2+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2012 Brennan Vincent <brennanv@email.arizona.edu>
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPLv2+"), or
 * the GNU Lesser General Public License Version 2 or later (the "LGPLv2+"),
 * in which case the provisions of the GPLv2+ or the LGPLv2+ are applicable
 * instead of those above.
 */

#include "MSPUBParser97.h"
#include "MSPUBCollector.h"

libmspub::MSPUBParser97::MSPUBParser97(WPXInputStream *input, MSPUBCollector *collector)
  : MSPUBParser2k(input, collector), m_isBanner(false)
{
}

bool libmspub::MSPUBParser97::parse()
{
  WPXInputStream *contents = m_input->getDocumentOLEStream("Contents");
  if (!contents)
  {
    MSPUB_DEBUG_MSG(("Couldn't get contents stream.\n"));
    return false;
  }
  if (!parseContents(contents))
  {
    MSPUB_DEBUG_MSG(("Couldn't parse contents stream.\n"));
    delete contents;
    return false;
  }
  return m_collector->go();
}

bool libmspub::MSPUBParser97::parseDocument(WPXInputStream *input)
{
  if (m_documentChunkIndex.is_initialized())
  {
    input->seek(m_contentChunks[m_documentChunkIndex.get()].offset + 0x12, WPX_SEEK_SET);
    unsigned short coordinateSystemMark = readU16(input);
    m_isBanner = coordinateSystemMark == 0x0007;
    unsigned width = readU32(input);
    unsigned height = readU32(input);
    m_collector->setWidthInEmu(width);
    m_collector->setHeightInEmu(height);
    return true;
  }
  return false;
}

int libmspub::MSPUBParser97::translateCoordinateIfNecessary(int coordinate) const
{
  if (m_isBanner)
  {
    return coordinate - 120 * EMUS_IN_INCH;
  }
  else
  {
    return coordinate - 25 * EMUS_IN_INCH;
  }
}

unsigned libmspub::MSPUBParser97::getFirstLineOffset() const
{
  return 0x22;
}

unsigned libmspub::MSPUBParser97::getSecondLineOffset() const
{
  return 0x2D;
}

unsigned libmspub::MSPUBParser97::getShapeFillTypeOffset() const
{
  return 0x20;
}

unsigned libmspub::MSPUBParser97::getShapeFillColorOffset() const
{
  return 0x18;
}

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */