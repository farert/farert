package org.sutezo.alps;

//package Route;


/*!	@file Route core logic implement.
 *	Copyright(c) sutezo9@me.com 2012.
 */

/*
 * 'Farert'
 * Copyright (C) 2014 Sutezo (sutezo666@gmail.com)
 *
 *    'Farert' is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     'Farert' is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with 'Farert'.  If not, see <http://www.gnu.org/licenses/>.
 *
 * このプログラムはフリーソフトウェアです。あなたはこれを、フリーソフトウェ
 * ア財団によって発行された GNU 一般公衆利用許諾契約書(バージョン3か、希
 * 望によってはそれ以降のバージョンのうちどれか)の定める条件の下で再頒布
 * または改変することができます。
 *
 * このプログラムは有用であることを願って頒布されますが、*全くの無保証*
 * です。商業可能性の保証や特定の目的への適合性は、言外に示されたものも含
 * め全く存在しません。詳しくはGNU 一般公衆利用許諾契約書をご覧ください。
 *
 * あなたはこのプログラムと共に、GNU 一般公衆利用許諾契約書の複製物を一部
 * 受け取ったはずです。もし受け取っていなければ、フリーソフトウェア財団ま
 * で請求してください
 */

 // station_id & line_id
 //
 class Station {
     public short lineId;
     public short stationId;

     public Station() {
         lineId = stationId = 0;
     }

     public Station(short lineId_, short stationId_) {
         lineId = lineId_;
         stationId = stationId_;
     }

     public Station(int lineId_, int stationId_) {
         this((short)lineId_, (short)stationId_);
     }

     void set(int lineId_, int stationId_) {
         lineId = (short)lineId_;
         stationId = (short)stationId_;
     }

     void let(final Station item) {
         lineId = item.lineId;
         stationId = item.stationId;
     }

     void set(final RouteItem routeItem) {
         lineId = routeItem.lineId;
         stationId = routeItem.stationId;
     }
     public boolean is_equal(final Station item_) {
         return lineId == item_.lineId &&
                 stationId == item_.stationId;
     }
     public boolean is_equal(final RouteItem item_) {
         return lineId == item_.lineId &&
                 stationId == item_.stationId;
     }
}
