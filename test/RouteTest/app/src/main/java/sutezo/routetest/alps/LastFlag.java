package sutezo.routetest.alps;

//package Route;

import java.util.*;

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

 class LastFlag implements Cloneable {
     // bit0-1

     private byte osakaKanPass = 0;

     // for debug print
     int getOsakaKanPassValue()      { return osakaKanPass; }

     enum OSAKAKAN_PASS {
         OSAKAKAN_NOPASS,	// 初期状態(大阪環状線未通過)
         OSAKAKAN_1PASS,	    // 大阪環状線 1回通過
         OSAKAKAN_2PASS, 	// 大阪環状線 2回通過
    };

     boolean is_osakakan_1pass() {
         return OSAKAKAN_PASS.OSAKAKAN_1PASS.ordinal() == (osakaKanPass & 0x03);
     }

     boolean is_osakakan_2pass() {
         return OSAKAKAN_PASS.OSAKAKAN_2PASS.ordinal() == (osakaKanPass & 0x03);
     }

     boolean is_osakakan_nopass() {
         return OSAKAKAN_PASS.OSAKAKAN_NOPASS.ordinal() == (osakaKanPass & 0x03);
     }

     void setOsakaKanPass(boolean value) {
         if (value) {
             osakaKanPass |= (1 << 0);
         } else {
             osakaKanPass &= ~(1 << 0);
         }
     }
     boolean getOsakaKanPass() { return 0 != (osakaKanPass & (1 << 0)); }

     // bit 2-3
     boolean osakakan_1dir = false;	//2 大阪環状線 1回目方向
     boolean osakakan_2dir = false;	//3 大阪環状線 2回目方向

     boolean osakakan_detour = false;   //4 大阪環状線 1回目遠回り(UI側から指定, 内部はR/O)

     void setOsakaKanFlag(OSAKAKAN_PASS pass) {
         this.osakaKanPass = ((byte) pass.ordinal());
     }

     void setOsakaKanFlag(LastFlag lf) {
         this.osakaKanPass = lf.osakaKanPass;
         this.osakakan_1dir = lf.osakakan_1dir;
         this.osakakan_2dir = lf.osakakan_2dir;
     }

     boolean trackmarkctl = false;	        //5 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
     boolean jctsp_route_change = false;	//6 分岐特例(add内部使用)

     // bit7 - 大高-杉本町とかで、[名][阪]をどっちに適用するか
     boolean meihancityflag= false;	    //7 ON: APPLIED_START / OFF:APPLIED_TERMINAL(User->System)
     boolean no_rule= false;               //8 ON: 特例非適用(User->System)
     boolean rule_en= false;               //9 ON: 特例適用(System->User)

     // bit 10 JR東海 東海道新幹線区間
     boolean jrtokaistock_enable= false;	//10 提案可能フラグ
     										//   東海道新幹線で[区][山][浜][京][阪]が適用する場面でセットされる
     										//   JR東海株主優待適用・非適用選択メニューが有効となる
     boolean jrtokaistock_applied= false; 		//11 提案適用フラグ
     										//   UI側からユーザが指定する

     // bit 11-20 発着 都区市内 適用
     boolean ter_begin_city		= false;      //13 [区][浜][名][京][阪][神][広][九][福][仙][札]
     boolean ter_fin_city		= false;      //14
     boolean ter_begin_yamate	= false;      //15/ [山]
     boolean ter_fin_yamate		= false;      //16
     boolean ter_begin_2city	= false;      //17 not used
     boolean ter_fin_2city		= false;      //18 not used
     boolean ter_begin_city_off	= false;      //19
     boolean ter_fin_city_off	= false;      //20
     boolean ter_begin_oosaka	= false;      //21 大阪・新大阪
     boolean ter_fin_oosaka		= false;      //22
     void terCityReset() {
         ter_begin_city		= false;      //13 [区][浜][名][京][阪][神][広][九][福][仙][札]
         ter_fin_city		= false;      //14
         ter_begin_yamate	= false;      //15/ [山]
         ter_fin_yamate		= false;      //16
         ter_begin_2city	= false;      //17 not used
         ter_fin_2city		= false;      //18 not used
         ter_begin_city_off	= false;      //19
         ter_fin_city_off	= false;      //20
         ter_begin_oosaka	= false;      //21 大阪・新大阪
         ter_fin_oosaka		= false;      //22
     }
     boolean isTerCity() {
         return
         ter_begin_city		||      //13 [区][浜][名][京][阪][神][広][九][福][仙][札]
         ter_fin_city		||      //14
         ter_begin_yamate	||      //15/ [山]
         ter_fin_yamate		||      //16
         ter_begin_2city	||      //17 not used
         ter_fin_2city		||      //18 not used
         ter_begin_city_off	||      //19
         ter_fin_city_off	||      //20
         ter_begin_oosaka	||      //21 大阪・新大阪
         ter_fin_oosaka		;      //22
     }

     // 会社線
     boolean compncheck	    	= false;      //23 会社線通過チェック有効
     boolean compnpass			= false;      //24 通過連絡運輸
     boolean compnda			= false;      //25 通過連絡運輸不正フラグ
     boolean compnbegin			= false;      //26	会社線で開始
     boolean compnend			= false;      //27 会社線で終了

     			// 28
     			// 29 used

     // Global
     boolean notsamekokurahakatashinzai = false;    //30 Route only : 小倉-博多間 新在別線扱い
     boolean end			= false;               //31 arrive to end.
     // end of last_flag

     LastFlag() {
         clear();
     }
     void clear() {
         /* last_flag */
         // boolean LASTFLG_OFF = RouteUtil.MASK(notsamekokurahakatashinzai);   // all bit clear at removeAll()
         osakaKanPass = 0;      //0-1

         // bit 2-3
         osakakan_1dir = false;	//2 大阪環状線 1回目方向
         osakakan_2dir = false;	//3 大阪環状線 2回目方向

         osakakan_detour = false;   //4 大阪環状線 1回目遠回り(UI側から指定, 内部はR/O)

         trackmarkctl = false;	        //5 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
         jctsp_route_change = false;	//6 分岐特例(add内部使用)

         meihancityflag = false;	    //7 ON: APPLIED_START / OFF:APPLIED_TERMINAL(User->System)
         no_rule = false;               //8 ON: 特例非適用(User->System)
         rule_en = false;               //9 ON: 特例適用(System->User)

         jrtokaistock_enable= false;	//10 提案可能フラグ
         jrtokaistock_applied= false; 		//11 提案適用フラグ

         ter_begin_city		= false;      //13 [区][浜][名][京][阪][神][広][九][福][仙][札]
         ter_fin_city		= false;      //14
         ter_begin_yamate	= false;      //15/ [山]
         ter_fin_yamate		= false;      //16
         ter_begin_2city	= false;      //17 not used
         ter_fin_2city		= false;      //18 not used
         ter_begin_city_off	= false;      //19
         ter_fin_city_off	= false;      //20
         ter_begin_oosaka	= false;      //21 大阪・新大阪
         ter_fin_oosaka		= false;      //22
         compncheck		    = false;      //23 会社線通過チェック有効
         compnpass			= false;      //24 通過連絡運輸
         compnda			= false;      //25 通過連絡運輸不正フラグ
         compnbegin			= false;      //26	会社線で開始
         compnend			= false;      //27 会社線で終了
         //notsamekokurahakatashinzai = false;    //30 Route only : 小倉-博多間 新在別線扱い
         end			    = false;      //31 arrive to end.
     }

     protected LastFlag clone() {
         try {
             return (LastFlag) super.clone();
         } catch (Exception e) {
             return new LastFlag();
         }
     }
 }
