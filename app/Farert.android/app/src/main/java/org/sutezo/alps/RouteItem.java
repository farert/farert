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
 ////////////////////////////////////////////
 //	RouteItem
 //
 public class RouteItem implements Cloneable {
     RouteItem() {}

     short lineId;
     short stationId;
     int flag;
     int city_no;      /* uint8_t in C++ */
     int urban_id;     /* uint8_t in C++ */

     public int lineId() { return lineId; }
     public int stationId() { return stationId; }

     // constructor
     RouteItem(int lineId_, int stationId_) {
         lineId = (short)lineId_;
         stationId = (short)stationId_;
         //    salesKm = fare = 0;

         //	if (lineId <= 0) {
         int[] dbrec = RouteUtil.AttrOfStationId(stationId_);
         if (dbrec.length != 3) {
             flag = (1<<30);
         } else {
             flag = dbrec[0] & RouteUtil.MASK_ROUTE_FLAG_SFLG;
             city_no = dbrec[1];
             urban_id = dbrec[2];
         }
         //	} else {
         //		flag = RouteUtil.AttrOfStationOnLineLine((int)lineId_, (int)stationId_);
         //	}
     }

     //	constructor
     //	@param [in] lineId_    路線
     //	@param [in] stationId_ 駅
     //	@param [in] flag_      masak(bit31-16のみ有効)
     //
     RouteItem(int lineId_, int stationId_, int flag_) {
         //    salesKm = fare = 0;

         lineId = (short)lineId_;
         stationId = (short)stationId_;
         int[] dbrec = RouteUtil.AttrOfStationId(stationId_);
         if (dbrec.length != 3) {
             flag = (1<<30);
         } else {
             flag = dbrec[0] & RouteUtil.MASK_ROUTE_FLAG_SFLG;
             city_no = dbrec[1];
             urban_id = dbrec[2];
         }
         flag |= (flag_ & RouteUtil.MASK_ROUTE_FLAG_LFLG);
     }

     RouteItem(final RouteItem ri) {
        let(ri);
     }

     // operator=
     void let(final RouteItem ri) {
         lineId = ri.lineId;
         stationId = ri.stationId;
         flag = ri.flag;
         city_no = ri.city_no;
     }

     public RouteItem clone() {
         try {
             RouteItem clone_obj = (RouteItem) super.clone();

             clone_obj.lineId = lineId;
             clone_obj.stationId = stationId;
             clone_obj.flag = flag;
             clone_obj.city_no = city_no;

             return clone_obj;
         } catch (CloneNotSupportedException e) {
             // 省略
         }
         return null;
     }


     void refresh() {
         int[] dbrec = RouteUtil.AttrOfStationId(stationId);
         if (dbrec.length != 3) {
             flag = (1<<30);
         } else {
             flag = dbrec[0] & RouteUtil.MASK_ROUTE_FLAG_SFLG;
             city_no = dbrec[1];
             urban_id = dbrec[2];
         }
     }

     boolean is_equal(RouteItem item_) {
         return lineId == item_.lineId &&
                 stationId == item_.stationId /* &&
                flag == item_.flag */;
     }
 }
