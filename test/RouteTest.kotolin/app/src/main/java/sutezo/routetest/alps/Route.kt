package sutezo.routetest.alps

 //package Route;

import java.util.*
import android.database.Cursor
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteOpenHelper


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

 /*
class PersonOpenHelper extends SQLiteOpenHelper {

	final static private int DB_VERSION = 1;

	public PersonOpenHelper(Context context) {
		super(context, null, null, DB_VERSION);
    }
	private static PersonOpenHelper helper = new PersonOpenHelper(this);
	_db = helper.getReadableDatabase();
}

*/


 class Route:RouteList() {
internal var routePassed:JctMask
init{
routePassed = JctMask()

}

@JvmOverloads internal fun assign(source_route:Route, count:Int = -1) {
route_list_raw = RouteList.dupRouteItems(source_route.route_list_raw, count)
lastFlag = source_route.lastFlag
reBuild()
}


internal class JCTSP_DATA {
 var jctSpMainLineId:Int = 0		// 分岐特例:本線(b)
 var jctSpStationId:Int = 0			// 分岐特例:分岐駅(c)
 var jctSpMainLineId2:Int = 0		// 分岐特例:本線(b)
 var jctSpStationId2:Int = 0		// 分岐特例:分岐駅(c)
init{
clear()
}
 fun clear() {
jctSpMainLineId = 0		// 分岐特例:本線(b)
jctSpStationId = 0			// 分岐特例:分岐駅(c)
jctSpMainLineId2 = 0		// 分岐特例:本線(b)
jctSpStationId2 = 0		// 分岐特例:分岐駅(c)
}
}

internal class JctMask {
 var jct_mask = CharArray(JCTMASKSIZE()) // about 40 byte
init{
clear()
}
 fun clear() {
for (i in jct_mask.indices)
{
jct_mask[i] = 0.toChar()
}
}
 fun on(jctid:Int) {
jct_mask[jctid / 8] = jct_mask[jctid / 8] or (1 shl jctid % 8).toChar()
}

 fun off(jctid:Int) {
jct_mask[jctid / 8] = jct_mask[jctid / 8] and (1 shl jctid % 8).inv().toChar()
}

 fun check(jctid:Int):Boolean {
return jct_mask[jctid / 8].toInt() and (1 shl jctid % 8) != 0
}
 fun at(index:Int):Char {
return jct_mask[index]
}
 fun or(index:Int, mask:Char) {
jct_mask[index] = jct_mask[index] or mask
}
 fun and(index:Int, mask:Char) {
jct_mask[index] = jct_mask[index] and mask.inv().toChar()
}
 fun assign(jctmask:JctMask) {
for (i in jct_mask.indices)
{
jct_mask[i] = jctmask.jct_mask[i]
}
}

companion object {
 fun JCTMASKSIZE():Int {
return (RouteUtil.MAX_JCT + 7) / 8
}
}
}


 ////////////////////////////////////////////////////////////////////////
    //
    //	Routeクラス インプリメント
    //

    /*!	@brief ルート作成(文字列からRouteオブジェクトの作成)
	 *
	 *	@param [in] route_str	カンマなどのデリミタで区切られた文字列("駅、路線、分岐駅、路線、..."）
	 *	@retval -200 failure(駅名不正)
	 *	@retval -300 failure(線名不正)
	 *	@retval -1   failure(経路重複不正)
	 *	@retval -2   failure(経路不正)
	 *	@retval 1 success
	 *	@retval 0 success
	 */
    internal fun setup_route(route_str:String):Int {
val token = "[, |/\t]+"
var lineId = 0
var stationId1 = 0
var stationId2 = 0
var rc = 1

removeAll()

val tok = route_str.split(token.toRegex()).dropLastWhile({ it.isEmpty() }).toTypedArray()
for (p in tok)
{
if (stationId1 == 0)
{
stationId1 = RouteUtil.GetStationId(p)
if (stationId1 <= 0)
{
rc = -200		/* illegal station name */
break
}
add(stationId1)
}
else if (lineId == 0)
{
if (p.get(0) == 'r')
{	/* 大阪環状線 遠回り */
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_OSAKAKAN_DETOUR)
lineId = RouteUtil.GetLineId(p.substring(1))
}
else
{
lineId = RouteUtil.GetLineId(p)
}
if (lineId <= 0)
{
rc = -300		/* illegal line name */
break
}
}
else
{
stationId2 = RouteUtil.GetStationId(p)
if (stationId2 <= 0)
{
rc = -200		/* illegal station name */
break
}
rc = add(lineId, /*stationId1,*/ stationId2)
 /* -1: route pass error
				 * -2: route error
				 * -100: db error
				 * 0: success(last)
				 * 1: success
                 * 4: company pass finish
				 */
                RouteUtil.ASSERT(rc == 0 || rc == 1 || rc == 10 || rc == 11 || rc == 4)
if (rc <= 0)
{
break
}
lineId = 0
stationId1 = stationId2
}
}
return rc
}

 // 大阪環状線 通過制御フラグ定義
 // ※ ここでいう「内回り」「外回り」は駅1-駅2間の進行方向ではなくDB定義上の
 //    順廻り／大回りのことを指す
 //                                    from  ex.
 // 0) 初期状態
 // 1) 近回り1回目 内回り                0 大阪→天王寺
 // 2) 近回り1回目 外回り                0 天王寺→西九条
 // 3) 近回り2回目 内回り - 内回り       1 今宮→大阪 … 京橋→天王寺
 // 4) 近回り2回目 外回り - 内回り       2 西九条→天王寺 … 京橋→大阪
 // 5) 近回り2回目 内回り - 外回り       1 大阪→京橋 … 天王寺→西九条
 // 6) 近回り2回目 外回り - 外回り       2 N/A(ありえへん)
 // 7) 遠回り指定 近回り1回目 内回り     1 大阪→天王寺 (1の時にUI指定した直後の状態)
 // 8) 遠回り指定 近回り1回目 外回り     2 天王寺→西九条 (1の時にUI指定した直後の状態)
 // 9) 遠回り指定 遠回り1回目 内回り     8 今宮→→京橋 (順廻りが遠回りの例)
 // a) 遠回り指定 遠回り1回目 外回り     7 大阪→→天王寺(福島軽油)
 // b) 遠回り指定 2回目 内回り - 内回り  9 今宮→→京橋 … 天王寺→京橋
 // c) 遠回り指定 2回目 外回り - 内回り  a 西九条→→京橋 … 大阪→西九条
 // d) 遠回り指定 2回目 内回り - 外回り  9 今宮→→京橋 … 天王寺→今宮
 // e) 遠回り指定 2回目 外回り - 外回り  a ありえへん？
 //
 // 遠回り指定は、UIから。1,2,9,aのみ許可(7,8はあり得ない)
 // 1,2は遠回りへ。 9,aは近回りへ


 // bit0-1: 回数 2ビット 0～2
 //        00=初期
 //        01=1回目
 //        10=2回目
 //        11=n/a
 // bit2: 1回目 内回り/外回り
 // bit3: 2回目 内回り/外回り
 // bit4: 遠回り指定


 //        計算時は、bit 2 BLF_OSAKAKAN_DETOUR のみ使用し、
 //		  計算の過程では、ローカル変数フラグ使用

    private fun chk_jctsb_b(kind:Int, num:Int):Boolean {
val dbid = DbidOf()
var ret = false

when (kind) {
RouteList.JCTSP_B_NISHIKOKURA ->
 /* 博多-新幹線-小倉*/
                ret = 2 <= num &&
dbid.LineIdOf_SANYOSHINKANSEN == route_list_raw[num - 1].lineId.toInt() &&
dbid.StationIdOf_KOKURA == route_list_raw[num - 1].stationId.toInt() &&
dbid.StationIdOf_HAKATA == route_list_raw[num - 2].stationId.toInt()
RouteList.JCTSP_B_YOSHIZUKA ->
 /* 小倉-新幹線-博多 */
                ret = 2 <= num &&
dbid.LineIdOf_SANYOSHINKANSEN == route_list_raw[num - 1].lineId.toInt() &&
dbid.StationIdOf_HAKATA == route_list_raw[num - 1].stationId.toInt() //&&
else -> {}
}//(0 < RouteUtil.InStation(dbid.StationIdOf_KOKURA,
 //                      dbid.LineIdOf_SANYOSHINKANSEN,
 //                      route_list_raw.get(num - 1).stationId,
 //                      route_list_raw.get(num - 2).stationId));
 //return (2 <= num) &&
 //	(dbid.LineIdOf_SANYOSHINKANSEN == route_list_raw.get(num - 1).lineId) &&
 //	(dbid.StationIdOf_HAKATA == route_list_raw.get(num - 1).stationId) &&
 //	(dbid.StationIdOf_KOKURA == route_list_raw.get(num - 2).stationId);
return ret
}


 //private:
    internal class RoutePass {
 //friend class Route;
         var routePassed = JctMask()

 var _source_jct_mask:JctMask? = null
 var _line_id:Int = 0
 var _station_id1:Int = 0
 var _station_id2:Int = 0
 var _start_station_id:Int = 0
 var _num:Int = 0		        // number of junction
 var _err:Boolean = false
 var _last_flag:Int = 0	// add() - removeTail() work

 fun clear() {
routePassed.clear()
_err = false
}
 fun update(source:RoutePass) {
routePassed.assign(source.routePassed)
if (null != source._source_jct_mask)
{
_source_jct_mask = JctMask()
_source_jct_mask!!.assign(source._source_jct_mask)
}
_line_id = source._line_id
_station_id1 = source._station_id1
_station_id2 = source._station_id2
_start_station_id = source._start_station_id
_num = source._num
_err = source._err
_last_flag = source._last_flag
}
 constructor() {} // default constructor
 //    public:
         constructor(source:RoutePass) {
update(source)
}

 //public
        // 経路マークリストコンストラクタ
        //
        //	@param [in]  jct_mask        分岐マーク(used check()) nullはremoveTail()用
        //	@param [in]  last_flag       制御フラグ
        //	@param [in]  line_id         路線
        //	@param [in]  station_id1	 発 or 至
        //	@param [in]  station_id2     至 or 発
        //
        @JvmOverloads  constructor(jct_mask:JctMask, last_flag:Int, line_id:Int, station_id1:Int, station_id2:Int, start_station_id:Int = 0 /* =0 */) {
routePassed = JctMask()
_source_jct_mask = jct_mask

_line_id = line_id
_station_id1 = station_id1
_station_id2 = station_id2
_start_station_id = start_station_id

_last_flag = last_flag

_err = false

if (station_id1 == station_id2)
{
_num = 0
}
else
{
if (line_id == DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN)
{
_num = enum_junctions_of_line_for_osakakan()
}
else
{
_num = enum_junctions_of_line()
}
}
}

 //	路線のbegin_station_id駅からto_station_id駅までの分岐駅リストを返す
        //
        //	注： lflg&(1<<17)を含めていないため、新幹線内分岐駅、たとえば、
        //	     東海道新幹線 京都 米原間に草津駅は存在するとして返します.
        //
        //	@return 分岐点数
        //
        private fun enum_junctions_of_line():Int {
var c:Int
val tsql = "select id from t_lines l join t_jct j on j.station_id=l.station_id where" +
"	line_id=?1 and (lflg&((1<<31)|(1<<15)))=(1<<15)" +
"	and sales_km>=" +
"			(select min(sales_km)" +
"			from t_lines" +
"			where line_id=?1" +
"			and (station_id=?2 or" +
"				 station_id=?3))" +
"	and sales_km<=" +
"			(select max(sales_km)" +
"			from t_lines" +
"			where line_id=?1" +
"			and (station_id=?2 or" +
"				 station_id=?3))" +
" order by" +
" case when" +
"	(select sales_km from t_lines where line_id=?1 and station_id=?3) <" +
"	(select sales_km from t_lines where line_id=?1 and station_id=?2)" +
" then sales_km" +
" end desc," +
" case when" +
"	(select sales_km from t_lines where line_id=?1 and station_id=?3) >" +
"	(select sales_km from t_lines where line_id=?1 and station_id=?2)" +
" then sales_km" +
" end asc"

val dbo = RouteDB.db().rawQuery(tsql, arrayOf(_line_id.toString(), _station_id1.toString(), _station_id2.toString()))
c = 0
try
{
++c
while (dbo.moveToNext())
{
routePassed.on(dbo.getInt(0))
c++
}
}
catch (e:Exception) {
c = -1
}
finally
{
dbo.close()
}
return c
}

 //	路線のbegin_station_id駅からto_station_id駅までの分岐駅リストを返す(大阪環状線DB上の逆回り(今宮経由))
        //
        //	@return 分岐点数
        //
        private fun enum_junctions_of_line_for_oskk_rev():Int {
var c:Int
val tsql = "select id from t_jct where station_id in ( " +
"select station_id from t_lines where line_id=?1 and (lflg&((1<<31)|(1<<15)))=(1<<15) and sales_km<= (select min(sales_km) from t_lines where " +
"line_id=?1 and (station_id=?2 or station_id=?3)) union all " +
"select station_id from t_lines where line_id=?1 and (lflg&((1<<31)|(1<<15)))=(1<<15) and sales_km>=(select max(sales_km) from t_lines where " +
"line_id=?1 and (station_id=?2 or station_id=?3)))"

RouteUtil.ASSERT(_line_id == DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN)

val dbo = RouteDB.db().rawQuery(tsql, arrayOf(_line_id.toString(), _station_id1.toString(), _station_id2.toString()))
c = 0

try
{
while (dbo.moveToNext())
{
routePassed.on(dbo.getInt(0))
c++
}
return c
}

finally
{
dbo.close()
}
 //return -1;	/* error */
        }


 //	RoutePass Constructor: 大阪環状線の乗車経路の分岐駅リストを返す
        //
        //	@return 分岐点数(plus and 0 or minus)
        //
        private fun enum_junctions_of_line_for_osakakan():Int {
var i:Int
var dir:Int
var jnum = -1
var check_result = 0
val farRoutePass = RoutePass(this)

RouteUtil.ASSERT(_line_id == DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN)

if (_source_jct_mask == null)
{
 /* removeTail() */
                if (RouteList.IS_LF_OSAKAKAN_PASS(_last_flag, RouteList.LF_OSAKAKAN_2PASS))
{
if (RouteUtil.BIT_CHK(_last_flag, RouteList.BLF_OSAKAKAN_2DIR))
{
jnum = enum_junctions_of_line_for_oskk_rev()
}
else
{
jnum = enum_junctions_of_line()
}
_last_flag = _last_flag and (RouteList.MLF_OSAKAKAN_PASS or (1 shl RouteList.BLF_OSAKAKAN_2DIR)).inv()
_last_flag = _last_flag or RouteList.LF_OSAKAKAN_1PASS

}
else if (RouteList.IS_LF_OSAKAKAN_PASS(_last_flag, RouteList.LF_OSAKAKAN_1PASS))
{
if (RouteUtil.BIT_CHK(_last_flag, RouteList.BLF_OSAKAKAN_1DIR))
{
jnum = enum_junctions_of_line_for_oskk_rev()
}
else
{
jnum = enum_junctions_of_line()
}
_last_flag = _last_flag and (RouteList.MLF_OSAKAKAN_PASS or (1 shl RouteList.BLF_OSAKAKAN_1DIR) or (1 shl RouteList.BLF_OSAKAKAN_2DIR) or (1 shl RouteList.BLF_OSAKAKAN_DETOUR)).inv()

}
else
{
 /* 一回も通っていないはあり得ない */
                    System.out.printf("osaka-kan pass flag is not 1 or 2 (%d)\n", _last_flag and RouteList.MLF_OSAKAKAN_PASS)
RouteUtil.ASSERT(false)
}
return jnum
}

dir = RouteUtil.DirOsakaKanLine(_station_id1, _station_id2)

if (RouteList.IS_LF_OSAKAKAN_PASS(_last_flag, RouteList.LF_OSAKAKAN_NOPASS))
{
 // 始めての大阪環状線
        		//    detour near far
        		// a:   1       o    o  far,1p,
        		// b:   1       -    x  far,  n/a あり得へんけどそのままret
        		// c:   1       x    o  far,2p
        		// d:   1       -    x  far,  n/a あり得へんけどそのままret
        		// e:   0       o    o  near,1p
        		// f:   0       o    x  near,2p
        		// g:   0       x    o  far, 2p
        		// h:   0       x    x  x
                if (RouteUtil.BIT_CHK(_last_flag, RouteList.BLF_OSAKAKAN_DETOUR))
{
 // 大回り指定
                    dir = dir xor 1
System.out.printf("Osaka-kan: 1far\n")
}
else
{
 // 通常(大回り指定なし)
                    System.out.printf("Osaka-kan: 1near\n")
}

 /* DB定義上の順廻り（内回り) : 外回り */
                if (0x01 and dir == 0)
{
jnum = enum_junctions_of_line()
System.out.printf("Osaka-kan: 2fwd\n")
}
else
{
jnum = enum_junctions_of_line_for_oskk_rev()
System.out.printf("Osaka-kan: 2rev\n")
}

 // 大阪環状線内駅が始発ポイントか見るため.
                // (始発ポイントなら1回通過でなく2回通過とするため)

                check_result = check() and 0x01
if (0x01 and check_result == 0 || !RouteUtil.BIT_CHK(_last_flag, RouteList.BLF_OSAKAKAN_DETOUR))
{
 // 近回りがOK または
                    // 近回りNGだが遠回り指定でない場合(大阪環状線内駅が始発ポイントである)
                    // a, c, e, f, g, h
                    if (0x01 and dir == 0)
{
 // 順廻り(DB上の. 実際には逆回り)が近回り.
                        // ...のときは遠回りを見てみる
                        i = farRoutePass.enum_junctions_of_line_for_oskk_rev()
System.out.printf("Osaka-kan: 3rev\n")
}
else
{
 // 逆回りが近回り
                        // ...のときは遠回りを見てみる
                        i = farRoutePass.enum_junctions_of_line()
System.out.printf("Osaka-kan: 3fwd\n")
}
}
else
{
 // b, d
                    // 遠回り指定がNG
                    //RouteUtil.ASSERT (false);
                    System.out.printf("Osaka-kan Illegal detour flag.\n")
return jnum
}
check_result = check_result or (farRoutePass.check() and 0x01 shl 4)
if (0x11 and check_result == 0)
{
 // a, e
					/* 両方向OK */
                    _last_flag = _last_flag and RouteList.MLF_OSAKAKAN_PASS.inv()
_last_flag = _last_flag or RouteList.LF_OSAKAKAN_1PASS
if (0x01 and dir == 0)
{
_last_flag = _last_flag and (1 shl RouteList.BLF_OSAKAKAN_1DIR).inv()
System.out.printf("Osaka-kan: 5fwd\n")
}
else
{
_last_flag = _last_flag or (1 shl RouteList.BLF_OSAKAKAN_1DIR)
System.out.printf("Osaka-kan: 5rev\n")
}
}
else if (0x11 and check_result != 0x11)
{
 // c, f, g
                    // 近回りか遠回りのどっちかダメ
                    if (0x11 and check_result == 0x01)
{
 // 近回りがNG
                        // f
                        dir = dir xor 1                        /* 戻す */
jnum = i
update(farRoutePass)	// 遠回りを採用
 // updateは_last_flagを壊す
                        System.out.printf("Osaka-kan: 6far\n")
}
else
{
 // c, g
                        System.out.printf("Osaka-kan: 6near\n")
}
_last_flag = _last_flag and RouteList.MLF_OSAKAKAN_PASS.inv()
_last_flag = _last_flag or RouteList.LF_OSAKAKAN_2PASS /* 大阪環状線駅始発 */
if (0x01 and dir == 0)
{
System.out.printf("Osaka-kan:7fwd\n")
_last_flag = _last_flag and (1 shl RouteList.BLF_OSAKAKAN_1DIR).inv()
}
else
{                                      // 計算時の経路上では1回目なので、
_last_flag = _last_flag or (1 shl RouteList.BLF_OSAKAKAN_1DIR)   // BLF_OSAKAKAN_2DIRでなく、BLF_OSAKAKAN_1DIR
System.out.printf("Osaka-kan:7rev\n")
}
}
else
{
 // どっち廻りもダメ(1回目でそれはない)
                    // h
                    // thru
                    System.out.printf("Osaka-kan:8\n")
 //RouteUtil.ASSERT (false); 大阪 東海道線 草津 草津線 柘植 関西線 今宮 大阪環状線 x鶴橋
                }
}
else if (RouteList.IS_LF_OSAKAKAN_PASS(_last_flag, RouteList.LF_OSAKAKAN_1PASS))
{

 // 2回目の大阪環状線
                // 最初近回りで試行しダメなら遠回りで。
                i = 0
while (i < 2)
{

clear()

if (0x01 and dir == 0)
{
jnum = enum_junctions_of_line()
System.out.printf("Osaka-kan: 9fwd\n")
}
else
{
jnum = enum_junctions_of_line_for_oskk_rev()
System.out.printf("Osaka-kan: 9rev\n")
}
if (0x01 and check() == 0 && (RouteUtil.STATION_IS_JUNCTION(_start_station_id) || _start_station_id == _station_id2 ||
InStationOnLine(DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN, _start_station_id) <= 0 ||
InStationOnOsakaKanjyou(dir, _start_station_id, _station_id1, _station_id2) <= 0))
{
 // 開始駅が大阪環状線なら、開始駅が通過範囲内にあるか ?
                        // 野田 大阪環状線 大阪 東海道線 尼崎 JR東西線 京橋 大阪環状線 福島(環)
                        // の2回目の大阪環状線は近回りの内回りは大阪でひっかかるが、外回りは分岐駅のみで判断
                        // すると通ってしまうので */
                        System.out.printf("Osaka-kan: 9(%d, %d, %d)\n", if (RouteUtil.STATION_IS_JUNCTION(_start_station_id)) 1 else 0, InStationOnLine(DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN, _start_station_id), InStationOnOsakaKanjyou(dir, _start_station_id, _station_id1, _station_id2))
System.out.printf("Osaka-kan: 9ok\n")
break /* OK */
}
else
{
_err = true	/* for jct_mask all zero and 0 < InStationOnOsakaKanjyou() */
}
 //System.out.printf("%d %d", RouteUtil.STATION_IS_JUNCTION(_start_station_id), _start_station_id);
                    //System.out.printf("Osaka-kan: @(%d, %d, %d)\n", check(), InStationOnLine(DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN, _start_station_id), InStationOnOsakaKanjyou(dir, _start_station_id, _station_id1, _station_id2));
                    System.out.printf("Osaka-kan: 9x\n")
dir = dir xor 0x1	/* 方向を逆にしてみる */
i++
} /* NG */

if (i < 2)
{
 /* OK */
                    _last_flag = _last_flag and RouteList.MLF_OSAKAKAN_PASS.inv()
_last_flag = _last_flag or RouteList.LF_OSAKAKAN_2PASS

if (dir and 0x01 == 0)
{
_last_flag = _last_flag and (1 shl RouteList.BLF_OSAKAKAN_2DIR).inv()
System.out.printf("Osaka-kan: 10ok_fwd\n")
}
else
{
_last_flag = _last_flag or (1 shl RouteList.BLF_OSAKAKAN_2DIR)
System.out.printf("Osaka-kan: 10ok_rev\n")
}
}
else
{
 /* NG */
                    System.out.printf("Osaka-kan: 11x\n")
}

}
else
{
 // 既に2回通っているので無条件で通過済みエラー
                System.out.printf("Osaka-kan: 3time\n")
jnum = enum_junctions_of_line()
RouteUtil.ASSERT(check() and 0x01 != 0)
enum_junctions_of_line_for_oskk_rev() // safety
RouteUtil.ASSERT(check() and 0x01 != 0)
}
System.out.printf("\ncheck:%d, far=%d, err=%d\n", check(), farRoutePass.check(), if (_err) 1 else 0)
return jnum	/* Failure */
}

 //public
        //	分岐駅リストの指定分岐idの乗車マスクをOff
        //
        //	@param [in]  jid   分岐id
        //
         fun off(jid:Int) {
routePassed.off(jid)
}

 //public
        //	分岐駅リストの乗車マスクをOff
        //
        //	@param [in]  jct_mask   分岐mask
        //
         fun off(jct_mask:JctMask) {
var i:Int

val count = JctMask.JCTMASKSIZE()
i = 0
while (i < count)
{
jct_mask.and(i, routePassed.at(i))

if (RouteDB.debug)
{
for (j in 0..7)
{
if (routePassed.at(i).toInt() and (1 shl j) != 0)
{
System.out.printf("removed.  : %s\n", JctName(i * 8 + j))
}
}
}
i++
}
}

 //public
        //	分岐駅リストの乗車マスクをOn
        //
        //	@param [in]  jct_mask   分岐mask
        //
         fun on(jct_mask:JctMask) {
var i:Int

val count = JctMask.JCTMASKSIZE()
i = 0
while (i < count)
{
jct_mask.or(i, routePassed.at(i))
if (RouteDB.debug)
{
for (j in 0..8)
{
if (1 shl j and routePassed.at(i).toInt() != 0)
{
System.out.printf("  add-mask on: %s(%d,%d)\n", JctName(i * 8 + j), Jct2id(i * 8 + j), i * 8 + j)
}
}
}
i++
}
}

 //public
        //	分岐駅リストの乗車マスクをOff
        //
        //	@param [in]  jct_mask   分岐mask
        //	@retval 0 = success
        //	@retval 1 = already passed.
        //	@retval 3 = already passed and last arrive point passed.
        //	@retval 2 = last arrive point.
        //
         fun check():Int {
var i:Int
var j:Int
var k:Int
var jctid:Int
var rc:Int

if (_source_jct_mask == null)
{
RouteUtil.ASSERT(false)
return -1	// for removeTail()
}
rc = 0
val count = JctMask.JCTMASKSIZE()
i = 0
while (i < count)
{
k = _source_jct_mask!!.at(i) and routePassed.at(i)
if (0 != k)
{
 /* 通過済みポイントあり */
                    j = 0
while (j < 8)
{
if (k and (1 shl j) != 0)
{
jctid = i * 8 + j /* 通過済みポイント */
if (Jct2id(jctid).toInt() == _station_id2)
{
rc = rc or 2   /* 終了駅 */
}
else if (Jct2id(jctid).toInt() != _station_id1)
{
rc = rc or 1	/* 既に通過済み */
System.out.printf("  already passed error: %s(%d,%d)\n", JctName(jctid), Jct2id(jctid), jctid)
break
}
}
j++
}
if (rc and 1 != 0)
{
break   /* 既に通過済み */
}
}
i++
}
if (rc and 0x01 == 0 && _err)
{
System.out.printf("Osaka-kan Pass check error.\n")
rc = rc or 1
}
return rc
}

 fun update_flag(source_flg:Int):Int {
var source_flg = source_flg
source_flg = source_flg and RouteList.LF_OSAKAKAN_MASK.inv()
source_flg = source_flg or (RouteList.LF_OSAKAKAN_MASK and _last_flag)
return source_flg
}
 fun num_of_junction():Int {
return _num
}

companion object {

 //static
        //	大阪環状線最短廻り方向を返す
        //
        //	@param [in]  station_id_a   発or至
        //	@param [in]  station_id_b   発or至
        // 	@retval 0 = 内回り(DB定義上の順廻り)が最短
        // 	@retval 1 = 外回りが最短
        //
        //	@note station_id_a, station_id_bは区別はなし
        //
         fun InStationOnOsakaKanjyou(dir:Int, start_station_id:Int, station_id_a:Int, station_id_b:Int):Int {
var n = 0

if (dir and 0x01 == 0)
{
n = RouteUtil.InStation(start_station_id, DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN, station_id_a, station_id_b)
}
else
{
val tsql = "select count(*) from (" +
"select station_id from t_lines where line_id=?1 and (lflg&(1<<31))=0 and sales_km<=(select min(sales_km) from t_lines where " +
"line_id=?1 and (station_id=?2 or station_id=?3)) union all " +
"select station_id from t_lines where line_id=?1 and (lflg&(1<<31))=0 and sales_km>=(select max(sales_km) from t_lines where " +
"line_id=?1 and (station_id=?2 or station_id=?3))" +
") where station_id=?4"

val dbo = RouteDB.db().rawQuery(tsql, arrayOf(String.valueOf(DbidOf.id().LineIdOf_OOSAKAKANJYOUSEN), station_id_a.toString(), station_id_b.toString(), start_station_id.toString()))
try
{
if (dbo.moveToNext())
{
n = dbo.getInt(0)
}
}

finally
{
dbo.close()
}
}
return n
}
}
} // class RoutePass


 //private:
    //	@brief	分岐マークOff
    //
    //	@param [in]  line_id          路線
    //	@param [in]  to_station_id    開始駅(含まない)
    //	@param [in]  begin_station_id 終了駅(含む)
    //
    //	@note add()で分岐特例経路自動変換時に使用大阪環状線はないものとする
    //
    internal fun routePassOff(line_id:Int, to_station_id:Int, begin_station_id:Int) {
val route_pass = RoutePass(routePassed, lastFlag, line_id, to_station_id, begin_station_id)
route_pass.off(routePassed)
lastFlag = route_pass.update_flag(lastFlag) /* update last_flag LF_OSAKAKAN_MASK */
}

 //	分岐駅が経路内に含まれているか？
    //
    //	@param [in] stationId   駅ident
    //	@retval true found
    //	@retval false notfound
    //
    internal fun junctionStationExistsInRoute(stationId:Int):Int {
var c:Int

c = 0
for (route_item in route_list_raw)
{
if (stationId == route_item.stationId.toInt())
{
c++
}
}
return c
}

 //private:
    //	大阪環状線経路変更による経路再構成(update rebuild last_flag)
    //
    //	@return 0 success(is last)
    //	@retval 1 success
    //	@retval otherwise failued
    //
    internal fun reBuild():Int {
val routeWork = Route()
var rc = 0

if (route_list_raw.size <= 1)
{
return 0
}

routeWork.add(route_list_raw[0].stationId.toInt())

 // add() の開始駅追加時removeAll()が呼ばれlast_flagがリセットされるため)
        routeWork.lastFlag = routeWork.lastFlag or (lastFlag and (1 shl RouteList.BLF_OSAKAKAN_DETOUR))

val pos = route_list_raw.iterator()
if (pos.hasNext())
{
pos.next()
}
while (pos.hasNext())
{
val ri = pos.next()
rc = routeWork.add(ri.lineId.toInt(), ri.stationId.toInt())
if (rc != RouteUtil.ADDRC_OK)
{
 /* error */
                break
}
}
if (rc < 0 || rc != RouteUtil.ADDRC_OK && rc == RouteUtil.ADDRC_LAST && pos.hasNext())
{
lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_OSAKAKAN_DETOUR)
return -1	/* error */
}

 // 経路自体は変わらない。フラグのみ
        // 特例適用、発着駅を単駅指定フラグは保持(大阪環状線廻りは既に持っている)
        lastFlag = routeWork.lastFlag or (lastFlag and (1 shl RouteList.BLF_MEIHANCITYFLAG or (1 shl RouteList.BLF_NO_RULE) or (1 shl RouteList.BLF_RULE_EN)))
routePassed.assign(routeWork.routePassed)

return rc
}

 //	遠回り/近回り設定
    //
    //	@return 0 success(is last)
    //	@retval 1 success
    //	@retval otherwise failued
    //
    internal fun setDetour():Int {
return setDetour(true)
}

 fun setDetour(enabled:Boolean):Int {
if (enabled)
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_OSAKAKAN_DETOUR)
}
else
{
lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_OSAKAKAN_DETOUR)
}
return reBuild()
}

 fun add(stationId:Int):Int {
removeAll(true)
route_list_raw.add(RouteItem(0.toShort().toInt(), stationId.toShort().toInt()))
System.out.printf("add-begin %s(%d)\n", RouteUtil.StationName(stationId), stationId)
return 1
}
@JvmOverloads  fun add(line_id:Int, stationId2:Int, ctlflg:Int = 0):Int {
var line_id = line_id
val dbid = DbidOf()
var rc:Int
var i:Int
val j:Int
var num:Int
var stationId1:Int
var lflg1:Int
var lflg2:Int
val start_station_id:Int
var replace_flg = false	// 経路追加ではなく置換
var jct_flg_on = 0   // 水平型検知(D-2) / BSRNOTYET_NA
var type = 0
val jctspdt = JCTSP_DATA()

 //if (RouteDB.debug) {
        val original_line_id = line_id
val first_station_id1:Int
 //}

        System.out.printf("last_flag=%x\n", lastFlag)

if (RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_END))
{
System.out.printf("route.add(): already terminated.\n")
return RouteUtil.ADDRC_END		/* already terminated. */
}

lastFlag = lastFlag and (1 shl RouteList.BLF_TRACKMARKCTL or (1 shl RouteList.BLF_JCTSP_ROUTE_CHANGE)).inv()

num = route_list_raw.size.toInt()
if (num <= 0)
{
RouteUtil.ASSERT(false)	// bug. must be call to add(station);
return -3
}
start_station_id = route_list_raw[0].stationId.toInt()
stationId1 = route_list_raw[route_list_raw.size - 1].stationId.toInt()
if (RouteDB.debug)
{
first_station_id1 = stationId1
}
 /* 発駅 */
        lflg1 = RouteUtil.AttrOfStationOnLineLine(line_id, stationId1)
if (RouteUtil.BIT_CHK(lflg1, RouteList.BSRNOTYET_NA))
{
return -2		/* 不正経路(line_idにstationId1は存在しない) */
}

 /* 着駅が発駅～最初の分岐駅間にあるか? */
		/* (着駅未指定, 発駅=着駅は除く) */
        /* Dec.2016: Remove teminal */
        //if ((num == 1) && (0 < end_station_id) && (end_station_id != start_station_id) &&
        //        (end_station_id != stationId2) &&
        //        (0 != RouteUtil.InStation(end_station_id, line_id, stationId1, stationId2))) {
        //    return -1;	/* <t> already passed error  */
        //}

        RouteUtil.ASSERT(RouteUtil.BIT_CHK(lflg1, RouteList.BSRJCTHORD) || route_list_raw[num - 1].stationId.toInt() == stationId1)

lflg2 = RouteUtil.AttrOfStationOnLineLine(line_id, stationId2)
if (RouteUtil.BIT_CHK(lflg2, RouteList.BSRNOTYET_NA))
{
return -2		/* 不正経路(line_idにstationId2は存在しない) */
}

 // 直前同一路線指定は受け付けない
        // 直前同一駅は受け付けない
        //if (!RouteUtil.BIT_CHK(route_list_raw.back().flag, BSRJCTHORD) && ((1 < num) && (line_id == route_list_raw.back().lineId))) {
        //	System.out.printf("iregal parameter by same line_id.\n");
        //	return -1;		// E-2, G, G-3, f, j1,j2,j3,j4 >>>>>>>>>>>>>>>>>>
        //}
        if (stationId1 == stationId2)
{
System.out.printf("iregal parameter by same station_id.\n")
System.out.printf("add_abort\n")
return -1		// E-112 >>>>>>>>>>>>>>>>>>
}

 // 同一路線で折り返した場合
        if (route_list_raw[num - 1].lineId.toInt() == line_id && 2 <= num &&
RouteUtil.DirLine(line_id, route_list_raw[num - 2].stationId.toInt(), stationId1) != RouteUtil.DirLine(line_id, stationId1, stationId2))
{
System.out.printf("re-route error.\n")
System.out.printf("add_abort\n")
return -1		//  >>>>>>>>>>>>>>>>>>
}

rc = companyPassCheck(line_id, stationId1, stationId2, num)
if (rc < 0)
{
return rc	/* 通過連絡運輸なし >>>>>>>>>>>> */
}

 // 分岐特例B(BSRJCTSP_B)水平型検知
        if (RouteUtil.BIT_CHK(lflg2, RouteList.BSRJCTSP_B) && chk_jctsb_b(type = GetBsrjctSpType(line_id, stationId2), num))
{
System.out.printf("JCT: h_(B)detect\n")
jct_flg_on = RouteUtil.BIT_ON(jct_flg_on, RouteList.BSRNOTYET_NA)	/* 不完全経路フラグ */
}
else
{
 /* 新幹線在来線同一視区間の重複経路チェック(lastItemのflagがBSRJCTHORD=ONがD-2ケースである */
            if (!RouteUtil.BIT_CHK(ctlflg, 8) &&
1 < num && !RouteUtil.BIT_CHK2(route_list_raw[num - 1].flag, RouteList.BSRJCTHORD, RouteList.BSRJCTSP_B) &&
!CheckTransferShinkansen(route_list_raw[num - 1].lineId.toInt(), line_id,
route_list_raw[num - 2].stationId.toInt(), stationId1, stationId2))
{
System.out.printf("JCT: F-3b\n")
return -1		// F-3b
}
}
System.out.printf("add %s(%d)-%s(%d), %s(%d)\n", RouteUtil.LineName(line_id), line_id, RouteUtil.StationName(stationId1), stationId1, RouteUtil.StationName(stationId2), stationId2)

if (RouteUtil.BIT_CHK(route_list_raw[num - 1].flag, RouteList.BSRJCTSP_B))
{
 /* 信越線上り(宮内・直江津方面) ? (フラグけちってるので
			  * t_jctspcl.type RetrieveJunctionSpecific()で吉塚、小倉廻りと区別しなければならない) */
            if (RouteUtil.LINE_DIR.LDIR_DESC == RouteUtil.DirLine(line_id, route_list_raw[num - 1].stationId.toInt(), stationId2) &&
(num < 2 || 2 <= num && RouteUtil.LINE_DIR.LDIR_ASC == RouteUtil.DirLine(route_list_raw[num - 1].lineId.toInt(),
route_list_raw[num - 2].stationId.toInt(),
route_list_raw[num - 1].stationId.toInt())) &&
RouteList.JCTSP_B_NAGAOKA == RetrieveJunctionSpecific(route_list_raw[num - 1].lineId.toInt(),
route_list_raw[num - 1].stationId.toInt(), jctspdt))
{
if (stationId2 == jctspdt.jctSpStationId2)
{ /* 宮内止まり？ */
System.out.printf("JSBH004\n")
System.out.printf("add_abort\n")
return -1
}
else
{
 // 長岡 → 浦佐
                    // 新幹線 長岡-浦佐をOff
                    routePassOff(route_list_raw[num - 1].lineId.toInt(),
route_list_raw[num - 1].stationId.toInt(),
jctspdt.jctSpStationId)
route_list_raw[num - 1].let(RouteItem(route_list_raw[num - 1].lineId.toInt(),
jctspdt.jctSpStationId.toShort().toInt()))
 // 上越線-宮内追加
                    rc = add(jctspdt.jctSpMainLineId, jctspdt.jctSpStationId2, ADD_BULLET_NC)		//****************
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
if (rc != RouteUtil.ADDRC_OK)
{
System.out.printf("junction special (JSBS001) error.\n")
System.out.printf("add_abort\n")
return rc			// >>>>>>>>>>>>>>>>>>>>>
}
num++
stationId1 = jctspdt.jctSpStationId2 // 宮内
 // line_id : 信越線
                    // stationId2 : 宮内～長岡
                }
}
else if (2 <= num && dbid.LineIdOf_SANYOSHINKANSEN == line_id)
{ /* b#14021205 add */
routePassed.off(Id2jctId(route_list_raw[num - 2].stationId.toInt()).toInt())
System.out.printf("b#14021205-1\n")
}
}
else if (2 <= num && RouteUtil.BIT_CHK(lflg2, RouteList.BSRJCTSP_B) &&	/* b#14021205 add */
dbid.LineIdOf_SANYOSHINKANSEN == route_list_raw[num - 1].lineId.toInt() &&
RouteUtil.DirLine(route_list_raw[num - 1].lineId.toInt(),
route_list_raw[num - 2].stationId.toInt(),
route_list_raw[num - 1].stationId.toInt()) != RouteUtil.DirLine(line_id, route_list_raw[num - 1].stationId.toInt(), stationId2))
{
routePassed.off(Id2jctId(route_list_raw[num - 1].stationId.toInt()).toInt())
System.out.printf("b#14021205-2\n")
jct_flg_on = RouteUtil.BIT_ON(jct_flg_on, RouteList.BSRNOTYET_NA)	/* 不完全経路フラグ */
}

 // 水平型検知
        if (RouteUtil.BIT_CHK(route_list_raw[num - 1].flag, RouteList.BSRJCTHORD))
{
System.out.printf("JCT: h_detect 2 (J, B, D)\n")
if (DbidOf.id().getStationIdOf_HAKATA() !== stationId1 && IsAbreastShinkansen(route_list_raw[num - 1].lineId.toInt(), line_id, stationId1, stationId2))
{
 // 	line_idは新幹線
                //	route_list_raw.get(num - 1).lineIdは並行在来線
                //
                RouteUtil.ASSERT(RouteUtil.IS_SHINKANSEN_LINE(line_id))
if (0 != RouteUtil.InStation(route_list_raw[num - 2].stationId.toInt(), line_id, stationId1, stationId2))
{
System.out.printf("JCT: D-2\n")
j = RouteUtil.NextShinkansenTransferTerm(line_id, stationId1, stationId2)
if (j <= 0)
{	// 隣駅がない場合
if (RouteDB.debug)
{
RouteUtil.ASSERT(original_line_id == line_id)
}
i = route_list_raw[num - 1].lineId.toInt()	// 並行在来線
 // 新幹線の発駅には並行在来線(路線b)に所属しているか?
                        if (0 == InStationOnLine(i, stationId2))
{
System.out.printf("prev station is not found in shinkansen.\n")
System.out.printf("add_abort\n")	// 恵那-名古屋-東京方面で、
 // 名古屋-三河安城間に在来線にない駅が存在し、
                            // その駅が着駅(stationId2)の場合
                            // ありえない
                            return -1			// >>>>>>>>>>>>>>>>>>>
}
else
{
System.out.printf("JCT: hor.(D-2x)\n")
removeTail()
num--
stationId1 = route_list_raw[route_list_raw.size - 1].stationId.toInt()
line_id = i
}
}
else
{
System.out.printf("JCT: hor.1(D-2)\n")
i = route_list_raw[num - 1].lineId.toInt()	// 並行在来線
if (InStationOnLine(i, j) <= 0)
{
System.out.printf("junction special (JSBX001) error.\n")
System.out.printf("add_abort\n")
return -1
}
removeTail()
rc = add(i, j, ADD_BULLET_NC)		//****************
RouteUtil.ASSERT(rc == RouteUtil.ADDRC_OK)
stationId1 = j
}
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
}
else
{
System.out.printf("JCT: B-2\n")
}
}
else
{
System.out.printf("JCT: J\n")
}
System.out.printf(">\n")
}

 // 151 check
        while (RouteUtil.BIT_CHK(lflg1, RouteList.BSRJCTSP))
{
System.out.printf(">\n")
 // 段差型
            if (RouteUtil.BIT_CHK(lflg2, RouteList.BSRJCTSP))
{	// 水平型でもある?
 // retrieve from a, d to b, c
                if (RouteDB.debug)
{
RouteUtil.ASSERT(original_line_id == line_id)
}
type = RetrieveJunctionSpecific(line_id, stationId2, jctspdt) // update jctSpMainLineId(b), jctSpStation(c)
RouteUtil.ASSERT(0 < type)
System.out.printf("JCT: detect step-horiz:%d\n", type)
if (jctspdt.jctSpStationId2 != 0)
{
lflg1 = RouteUtil.BIT_OFF(lflg1, RouteList.BSRJCTSP)				// 別に要らないけど
break
}
}
if (RouteDB.debug)
{
RouteUtil.ASSERT(original_line_id == line_id)
RouteUtil.ASSERT(first_station_id1 == stationId1)
}
 // retrieve from a, d to b, c
            type = RetrieveJunctionSpecific(line_id, stationId1, jctspdt) // update jctSpMainLineId(b), jctSpStation(c)
RouteUtil.ASSERT(0 < type)
System.out.printf("JCT: detect step:%d\n", type)
if (stationId2 != jctspdt.jctSpStationId)
{
if (route_list_raw[num - 1].lineId.toInt() == jctspdt.jctSpMainLineId)
{
RouteUtil.ASSERT(stationId1 == route_list_raw[num - 1].stationId.toInt())
if (0 < RouteUtil.InStation(jctspdt.jctSpStationId,
route_list_raw[num - 1].lineId.toInt(),
route_list_raw[num - 2].stationId.toInt(),
stationId1))
{
System.out.printf("JCT: C-1\n")
routePassOff(jctspdt.jctSpMainLineId, jctspdt.jctSpStationId, stationId1)	// C-1
}
else
{ // A-1
System.out.printf("JCT: A-1\n")
}
if (2 <= num && jctspdt.jctSpStationId == route_list_raw[num - 2].stationId.toInt())
{
removeTail()
System.out.printf("JCT: A-C\n")		// 3, 4, 8, 9, g,h
--num
}
else
{
route_list_raw[num - 1].let(RouteItem(route_list_raw[num - 1].lineId.toInt(),
jctspdt.jctSpStationId.toShort().toInt()))
System.out.printf("JCT: %d\n", 4485 /*__LINE__*/)
}
if (jctspdt.jctSpStationId2 != 0)
{		// 分岐特例路線2
System.out.printf("JCT: step_(2)detect\n")
rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId2, ADD_BULLET_NC)	//**************
RouteUtil.ASSERT(rc == RouteUtil.ADDRC_OK)
num++
if (rc != RouteUtil.ADDRC_OK)
{			// safety
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
System.out.printf("A-1/C-1(special junction 2)\n")
System.out.printf("add_abort\n")
return -1
}
if (stationId2 == jctspdt.jctSpStationId2)
{
System.out.printf("KF1,2\n")
line_id = jctspdt.jctSpMainLineId2
replace_flg = true
}
stationId1 = jctspdt.jctSpStationId2
}
else
{
stationId1 = jctspdt.jctSpStationId
}
}
else
{
if (RouteDB.debug)
{
RouteUtil.ASSERT(first_station_id1 == stationId1)
}
if (num < 2 ||
!IsAbreastShinkansen(jctspdt.jctSpMainLineId,
route_list_raw[num - 1].lineId.toInt(),
stationId1,
route_list_raw[num - 2].stationId.toInt())
|| jctspdt.jctSpStationId == dbid.StationIdOf_NISHIKOKURA // KC-2

|| jctspdt.jctSpStationId == dbid.StationIdOf_YOSHIZUKA // KC-2

|| RouteUtil.InStation(jctspdt.jctSpStationId,
route_list_raw[num - 1].lineId.toInt(),
route_list_raw[num - 2].stationId.toInt(),
stationId1) <= 0)
{
 // A-0, I, A-2
                        System.out.printf("JCT: A-0, I, A-2\n")	//***************

if (jctspdt.jctSpStationId == dbid.StationIdOf_NISHIKOKURA // KC-2
 || jctspdt.jctSpStationId == dbid.StationIdOf_YOSHIZUKA)
{ // KC-2
routePassOff(jctspdt.jctSpMainLineId,
stationId1, jctspdt.jctSpStationId)
System.out.printf("JCT: KC-2\n")
}
rc = add(jctspdt.jctSpMainLineId,
 /*route_list_raw.get(num - 1).stationId,*/ jctspdt.jctSpStationId,
ADD_BULLET_NC)
RouteUtil.ASSERT(rc == RouteUtil.ADDRC_OK)
num++
if (rc != RouteUtil.ADDRC_OK)
{				// safety
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
System.out.printf("A-0, I, A-2\n")
System.out.printf("add_abort\n")
return -1					//>>>>>>>>>>>>>>>>>>>>>>>>>>
}
if (jctspdt.jctSpStationId2 != 0)
{		// 分岐特例路線2
rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId2, ADD_BULLET_NC)	//**************
num++
RouteUtil.ASSERT(rc == RouteUtil.ADDRC_OK)
if (rc != RouteUtil.ADDRC_OK)
{			// safety
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
System.out.printf("A-0, I, A-2(special junction 2)\n")
System.out.printf("add_abort\n")
return -1				//>>>>>>>>>>>>>>>>>>>>>>>>>>
}
if (stationId2 == jctspdt.jctSpStationId2)
{
System.out.printf("KF0,3,4\n")
line_id = jctspdt.jctSpMainLineId2
replace_flg = true
}
stationId1 = jctspdt.jctSpStationId2
}
else
{
stationId1 = jctspdt.jctSpStationId
}
}
else
{
if (RouteDB.debug)
{
RouteUtil.ASSERT(first_station_id1 == stationId1)
}
 // C-2
                        System.out.printf("JCT: C-2\n")
RouteUtil.ASSERT(RouteUtil.IS_SHINKANSEN_LINE(route_list_raw[num - 1].lineId.toInt()))
routePassOff(jctspdt.jctSpMainLineId,
jctspdt.jctSpStationId, stationId1)
i = RouteUtil.NextShinkansenTransferTerm(route_list_raw[num - 1].lineId.toInt(), stationId1, route_list_raw[num - 2].stationId.toInt())
if (i <= 0)
{	// 隣駅がない場合
System.out.printf("JCT: C-2(none next station on bullet line)\n")
 // 新幹線の発駅には並行在来線(路線b)に所属しているか?
                            if (0 == InStationOnLine(jctspdt.jctSpMainLineId,
route_list_raw[num - 2].stationId.toInt()))
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
System.out.printf("next station is not found in shinkansen.\n")
System.out.printf("add_abort\n")
return -1			// >>>>>>>>>>>>>>>>>>>
}
else
{
removeTail()
rc = add(jctspdt.jctSpMainLineId,
jctspdt.jctSpStationId, ADD_BULLET_NC)	//**************
RouteUtil.ASSERT(rc == RouteUtil.ADDRC_OK)
stationId1 = jctspdt.jctSpStationId
}
}
else
{
route_list_raw[num - 1].let(RouteItem(route_list_raw[num - 1].lineId.toInt(), i.toShort().toInt()))
route_list_raw.add(RouteItem(jctspdt.jctSpMainLineId.toShort().toInt(),
jctspdt.jctSpStationId.toShort().toInt()))
stationId1 = jctspdt.jctSpStationId
}
}
}
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
}
else
{
 // E, G		(stationId2 == jctspdt.jctSpStationId)
                System.out.printf("JCT: E, G\n")
if (jctspdt.jctSpStationId2 != 0)
{
System.out.printf("JCT: KE0-4\n")
lflg2 = RouteUtil.BIT_OFF(lflg2, RouteList.BSRJCTSP)
}
line_id = jctspdt.jctSpMainLineId
if (RouteDB.debug)
{
RouteUtil.ASSERT(first_station_id1 == stationId1)
}
if (2 <= num &&
 //			!RouteUtil.BIT_CHK(RouteUtil.AttrOfStationOnLineLine(line_id, stationId2), BSRJCTSP_B) &&
                        0 < RouteUtil.InStation(stationId2, jctspdt.jctSpMainLineId,
route_list_raw[num - 2].stationId.toInt(), stationId1))
{
System.out.printf("E-3:duplicate route error.\n")
System.out.printf("add_abort\n")
return -1	// Duplicate route error >>>>>>>>>>>>>>>>>
}
if (RouteUtil.BIT_CHK(RouteUtil.AttrOfStationOnLineLine(line_id, stationId2), RouteList.BSRJCTSP_B))
{
 // 博多-小倉-西小倉
                    //
                    System.out.printf("jct-b nisi-kokura-stop/yoshizuka-stop\n")
}
if (route_list_raw[num - 1].lineId.toInt() == jctspdt.jctSpMainLineId)
{
 // E-3 , B-0, 5, 6, b, c, d, e
                    // E-0, E-1, E-1a, 6, b, c, d, e
                    System.out.printf("JCT: E-3, B0,5,6,b,c,d,e, E-0,E-1,E-1a,6,b,c,d,e\n")
replace_flg = true
}
else
{
 // E-2, G, G-3, G-2, G-4
                    System.out.printf("JCT: E-2, G, G-3, G-2, G-4\n")
}
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
}
break
}
if (RouteUtil.BIT_CHK(lflg2, RouteList.BSRJCTSP))
{
 // 水平型
            // a(line_id), d(stationId2) -> b(jctSpMainLineId), c(jctSpStationId)
            if (RouteDB.debug)
{
RouteUtil.ASSERT(original_line_id == line_id)
 //RouteUtil.ASSERT (first_station_id1 == stationId2);
            }
type = RetrieveJunctionSpecific(line_id, stationId2, jctspdt)
RouteUtil.ASSERT(0 < type)
System.out.printf("JCT:%d\n", type)
if (stationId1 == jctspdt.jctSpStationId)
{
 // E10-, F, H
                System.out.printf("JCT: E10-, F, H/KI0-4\n")
line_id = jctspdt.jctSpMainLineId	// a -> b
if (route_list_raw[num - 1].lineId.toInt() == jctspdt.jctSpMainLineId)
{
if (2 <= num && 0 < RouteUtil.InStation(stationId2, jctspdt.jctSpMainLineId,
route_list_raw[num - 2].stationId.toInt(), stationId1))
{
System.out.printf("E11:duplicate route error.\n")
System.out.printf("add_abort\n")
return -1	// Duplicate route error >>>>>>>>>>>>>>>>>
}
replace_flg = true
System.out.printf("JCT: F1, H, E11-14\n")
}
else
{
 // F-3bはエラーとするため. last_flag = RouteUtil.BIT_ON(jct_flg_on, BSRJCTHORD);	// F-3b
                }
}
else
{
 // J, B, D
                if (jctspdt.jctSpStationId2 != 0 && stationId1 == jctspdt.jctSpStationId2)
{	// 分岐特例路線2
System.out.printf("JCT: KJ0-4(J, B, D)\n")
rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId, ADD_BULLET_NC)		//**************
num++
if (rc != RouteUtil.ADDRC_OK)
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
System.out.printf("junction special (KJ) error.\n")
System.out.printf("add_abort\n")
return rc			// >>>>>>>>>>>>>>>>>>>>>
}
}
else
{
if (jctspdt.jctSpStationId2 != 0)
{	// 分岐特例路線2
System.out.printf("JCT: KH0-4(J, B, D) add(日田彦山線, 城野c')\n")
rc = add(line_id, /*stationId1,*/ jctspdt.jctSpStationId2, ADD_BULLET_NC)	//**************
num++
if (rc == RouteUtil.ADDRC_OK)
{
System.out.printf("JCT: add(日豊線b', 西小倉c)\n")
rc = add(jctspdt.jctSpMainLineId2, jctspdt.jctSpStationId, ADD_BULLET_NC)	//**************
num++
}
if (rc != RouteUtil.ADDRC_OK)
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
System.out.printf("junction special horizen type convert error.\n")
System.out.printf("add_abort\n")
return rc			// >>>>>>>>>>>>>>>>>>>>>
}
}
else
{
System.out.printf("JCT: J, B, D\n")
rc = add(line_id, /*stationId1,*/ jctspdt.jctSpStationId, ADD_BULLET_NC)	//**************
num++
if (rc != RouteUtil.ADDRC_OK)
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
System.out.printf("junction special horizen type convert error.\n")
System.out.printf("add_abort\n")
return rc			// >>>>>>>>>>>>>>>>>>>>>
}
if (stationId2 != dbid.StationIdOf_KOKURA)
{
 // b#15032701
                            jct_flg_on = RouteUtil.BIT_ON(jct_flg_on, RouteList.BSRJCTHORD)	//b#14021202
}
}
}
 // b#14021202 last_flag = RouteUtil.BIT_ON(jct_flg_on, BSRJCTHORD);
                line_id = jctspdt.jctSpMainLineId
stationId1 = jctspdt.jctSpStationId
}
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
}

 // 長岡周りの段差型
        if (2 <= num && RouteUtil.BIT_CHK(lflg1, RouteList.BSRJCTSP_B))
{
if (RouteList.JCTSP_B_NAGAOKA == RetrieveJunctionSpecific(line_id,
route_list_raw[num - 1].stationId.toInt(), jctspdt))
{
 /* 信越線下り(直江津→長岡方面) && 新幹線|上越線上り(長岡-大宮方面)? */
                if (RouteUtil.LINE_DIR.LDIR_ASC == RouteUtil.DirLine(route_list_raw[num - 1].lineId.toInt(),
route_list_raw[num - 2].stationId.toInt(),
route_list_raw[num - 1].stationId.toInt()) && RouteUtil.LINE_DIR.LDIR_DESC == RouteUtil.DirLine(line_id,
route_list_raw[num - 1].stationId.toInt(),
stationId2))
{
 /* 宮内発 */
                    if (route_list_raw[num - 2].stationId.toInt() == jctspdt.jctSpStationId2)
{
System.out.printf("junction special 2(JSBS004) error.\n")
System.out.printf("add_abort\n")
return -1			// >>>>>>>>>>>>>>>>>>>>>
}
 // 長岡Off
                    routePassed.off(Id2jctId(route_list_raw[num - 1].stationId.toInt()).toInt())

 // 長岡->宮内へ置換
                    route_list_raw[num - 1].let(RouteItem(route_list_raw[num - 1].lineId.toInt(),
jctspdt.jctSpStationId2.toShort().toInt()))

 // 上越線 宮内→浦佐
                    rc = add(jctspdt.jctSpMainLineId, jctspdt.jctSpStationId, ADD_BULLET_NC)		//****************
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
if (RouteUtil.ADDRC_OK != rc)
{
System.out.printf("junction special 2(JSBH001) error.\n")
System.out.printf("add_abort\n")
return rc			// >>>>>>>>>>>>>>>>>>>>>
}
stationId1 = jctspdt.jctSpStationId
num += 1
}
}
}

val route_pass = RoutePass(routePassed, lastFlag, line_id, stationId1, stationId2, start_station_id)
if (route_pass.num_of_junction() < 0)
{
System.out.printf("DB error(add-junction-enum)\n")
System.out.printf("add_abort\n")
RouteUtil.ASSERT(false)
return -100		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
}

 // Route passed check
        rc = route_pass.check()

if (line_id == dbid.LineIdOf_OOSAKAKANJYOUSEN)
{
if (rc and 0x01 != 0)
{
rc = -1
}
else if (rc and 0x02 != 0)
{
 /* 着駅終了*/
                if (RouteUtil.STATION_IS_JUNCTION_F(lflg2))
{
rc = 1		/* BLF_TRACKMARKCTL をONにして、次のremoveTail()で削除しない */
System.out.printf("osaka-kan last point junction\n")
}
else
{
System.out.printf("osaka-kan last point not junction\n")
rc = 2
}
}
else if (rc and 0x03 == 0)
{
if (2 <= route_list_raw.size && route_list_raw[1].lineId.toInt() == line_id &&
!RouteUtil.STATION_IS_JUNCTION(start_station_id) && start_station_id != stationId2 &&
0 != RouteUtil.InStation(stationId2, line_id, start_station_id, route_list_raw[1].stationId.toInt()))
{
System.out.printf("osaka-kan passed error\n")	// 要るか？2015-2-15
rc = -1	/* error */
rc = 0
}
else if (start_station_id == stationId2)
{
rc = 1
}
else
{
rc = 0	/* OK */
}
}
else
{
RouteUtil.ASSERT(false)	// rc & 0x03 = 0x03
rc = -1		// safety
}
}
else if (rc == 0)
{	// 復乗なし
if (2 <= route_list_raw.size && start_station_id != stationId2 &&
0 != RouteUtil.InStation(start_station_id, line_id, stationId1, stationId2))
{
rc = -1	/* <v> <p> <l> <w> */
}
else if (start_station_id == stationId2)
{
rc = 2		/* <f> */
}
else
{
rc = 0		/* <a> <d> <g> */
}/* =Dec.2016:Remove terminal= ||
    			(((0 < end_station_id) && (end_station_id != stationId2) && (2 <= route_list_raw.size())) &&
    			(0 != RouteList::InStation(end_station_id, line_id, stationId1, stationId2)))*/
}
else
{	// 復乗
if (rc and 1 == 0)
{ /* last */
if (
 //	(!RouteUtil.STATION_IS_JUNCTION(stationId2)) ||
                    // sflg.12は特例乗り換え駅もONなのでlflg.15にした
                    !RouteUtil.STATION_IS_JUNCTION_F(lflg2) || 2 <= num && start_station_id != stationId2 &&
0 != RouteUtil.InStation(start_station_id, line_id, stationId1, stationId2))
{
rc = -1	/* <k> <e> <r> <x> <u> <m> */
}
else
{
rc = 1		/* <b> <j> <h> */
}/* =Dec.2016:Reomve terminal=  ||
            		(((0 < end_station_id) && (end_station_id != stationId2) && (2 <= num)) &&
            		(0 != RouteList::InStation(end_station_id, line_id, stationId1, stationId2)))
            		*/
}
else
{
rc = -1	/* 既に通過済み */
}
}

if (rc < 0)
{
 // 不正ルートなのでmaskを反映しないで破棄する

            System.out.printf("add_abort(%d)\n", rc)
lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_TRACKMARKCTL)
 // E-12, 6, b, c, d, e
            return -1	/* already passed error >>>>>>>>>>>>>>>>>>>> */

}
else
{
 // normality
            // 通過bit ON(maskをマージ)
            route_pass.on(routePassed)

 // 大阪環状線通過フラグを設定
            lastFlag = route_pass.update_flag(lastFlag) /* update last_flag LF_OSAKAKAN_MASK */
}

 /* 追加か置換か */
        if (replace_flg)
{
RouteUtil.ASSERT(0 < type)	// enable jctspdt
RouteUtil.ASSERT(line_id == jctspdt.jctSpMainLineId || line_id == jctspdt.jctSpMainLineId2)
RouteUtil.ASSERT(route_list_raw[num - 1].lineId.toInt() == jctspdt.jctSpMainLineId || route_list_raw[num - 1].lineId.toInt() == jctspdt.jctSpMainLineId2)
route_list_raw.removeAt(route_list_raw.size - 1)
--num
}
lflg1 = RouteUtil.AttrOfStationOnLineLine(line_id, stationId1)
lflg2 = RouteUtil.AttrOfStationOnLineLine(line_id, stationId2)

lflg2 = lflg2 or (lflg1 and 0xff000000.toInt())
lflg2 = lflg2 and (0xff00ffff.toInt() and (1 shl RouteList.BSRJCTHORD).inv())	// 水平型検知(D-2);
lflg2 = lflg2 or jct_flg_on	// BSRNOTYET_NA:BSRJCTHORD
route_list_raw.add(RouteItem(line_id.toShort().toInt(), stationId2.toShort().toInt(), lflg2))
++num

if (rc == 0)
{
System.out.printf("added continue.\n")
lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_TRACKMARKCTL)
}
else if (rc == 1)
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_TRACKMARKCTL)
}
else if (rc == 2)
{
lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_TRACKMARKCTL)	/* 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので) */
}
else
{
RouteUtil.ASSERT(false)
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_TRACKMARKCTL)
}

System.out.printf("added fin.(%d).\n", rc)

if (rc != 0)
{
if (RouteUtil.BIT_CHK(lflg2, RouteList.BSRNOTYET_NA))
{
System.out.printf("？？？西小倉・吉塚.rc=%d\n", rc)
return RouteUtil.ADDRC_OK	/* 西小倉、吉塚 */
}
else
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_END)
System.out.printf("detect finish.\n")
return RouteUtil.ADDRC_LAST
}
}
else
{
if (RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_COMPNDA))
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_END)
return RouteUtil.ADDRC_CEND
}
else
{
return RouteUtil.ADDRC_OK	/* OK - Can you continue */
}
}
}

@JvmOverloads  fun removeTail(begin_off:Boolean = false/* = false*/) {
val line_id:Int
val begin_station_id:Int
val to_station_id:Int
var i:Int
val list_num:Int

System.out.printf("Enter removeTail\n")

list_num = route_list_raw.size.toInt()
if (list_num < 2)
{
lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_TRACKMARKCTL)
return
}

 /* 発駅～最初の乗換駅時 */
        if (list_num <= 2)
{
removeAll(false, false)
return
}

line_id = route_list_raw[list_num - 1].lineId.toInt()

to_station_id = route_list_raw[list_num - 1].stationId.toInt()	// tail
begin_station_id = route_list_raw[list_num - 2].stationId.toInt()	// tail - 1

val route_pass = RoutePass(null, lastFlag, line_id, to_station_id, begin_station_id)

if (!begin_off)
{
 /* 開始駅はOffしない(前路線の着駅なので) */
            i = Id2jctId(begin_station_id).toInt()
if (0 < i)
{
route_pass.off(i)
}
}

i = Id2jctId(to_station_id).toInt()
if (0 < i && RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_TRACKMARKCTL))
{
 /* 最近分岐駅でO型経路、P型経路の着駅の場合は除外 */
            route_pass.off(i)
}

route_pass.off(routePassed)

lastFlag = route_pass.update_flag(lastFlag) /* update last_flag LF_OSAKAKAN_MASK */
lastFlag = lastFlag and (1 shl RouteList.BLF_TRACKMARKCTL or (1 shl RouteList.BLF_END)).inv()

if (RouteUtil.IS_COMPANY_LINE(route_list_raw[route_list_raw.size - 1].lineId.toInt()))
{
if (!RouteUtil.IS_COMPANY_LINE(route_list_raw[list_num - 2].lineId.toInt()))
{
lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_COMPNCHECK)
}
}

route_list_raw.removeAt(route_list_raw.size - 1)	// route_list_raw.pop_back();

 /* 後半リストチェック */
    	if (RouteUtil.IS_COMPANY_LINE(route_list_raw[route_list_raw.size - 1].lineId.toInt()))
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_COMPNEND)
lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_COMPNPASS)
}
else
{
lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_COMPNEND)
}
lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_COMPNDA)
}

@JvmOverloads  fun removeAll(bWithStart:Boolean = true /* =true */, bWithEnd:Boolean = true /* =true */) {
var begin_station_id = 0

routePassed.clear()

if (!bWithStart)
{
begin_station_id = startStationId()
}

route_list_raw.clear()
lastFlag = RouteList.LASTFLG_OFF

System.out.printf("clear-all mask.\n")

if (!bWithStart)
{
add(begin_station_id)
}
}

 //public:
    //	経路を逆転
    //
    //	@retval 1   sucess
    //	@retval 0   sucess(empty)
    //	@retval -1	failure(6の字を逆転すると9になり経路重複となるため)
    //
     fun reverse():Int {
var station_id:Int
var line_id:Int
val route_list_rev = ArrayList<RouteItem>(route_list_raw.size)

if (route_list_raw.size <= 1)
{
return 0
}

val revIterator = route_list_raw.listIterator(route_list_raw.size)
while (revIterator.hasPrevious())
{
val ri = revIterator.previous()
route_list_rev.add(ri)
}

removeAll()	/* clear route_list_raw */

var ite = route_list_rev.listIterator()
if (ite.hasNext())
{
var ri = ite.next()
station_id = ri.stationId.toInt()
add(station_id)
line_id = ri.lineId.toInt()
while (ite.hasNext())
{
ri = ite.next()
var rc = add(line_id, /*station_id,*/ ri.stationId.toInt())
if (rc < 0)
{
 /* error */
					/* restore */

                    removeAll()	/* clear route_list_raw */

ite = route_list_rev.listIterator(route_list_rev.size)
if (ite.hasPrevious())
{
ri = ite.previous()
station_id = ri.stationId.toInt()
add(station_id)
while (ite.hasPrevious())
{
ri = ite.previous()
rc = add(ri.lineId.toInt(), /*station_id,*/ ri.stationId.toInt(), 1 shl 8)
RouteUtil.ASSERT(0 <= rc)
}
}
else
{
RouteUtil.ASSERT(false)
}
System.out.printf("cancel reverse route\n")
return -1
}
station_id = ri.stationId.toInt()
line_id = ri.lineId.toInt()
}
}
else
{
RouteUtil.ASSERT(false)
}
System.out.printf("reverse route\n")
return 1
}

 //public:
    //
    //	最短経路に変更(raw immidiate)
    //
    //	@param [in] useBulletTrain (boolean)新幹線使用有無
    //	@retval true success
    //	@retval 1 : success
    //	@retval 0 : loop end.
    //	@retval 4 : already routed
    //	@retval 5 : already finished
    //	@retval -n: add() error(re-track)
    //	@retval -32767 unknown error(DB error or BUG)
    //
     fun changeNeerest(useBulletTrain:Boolean, end_station_id:Int):Int {
class Dijkstra {
internal inner class NODE_JCT {
 var minCost:Int = 0
 var fromNode:Short = 0
 var done_flg:Boolean = false
 var line_id:Short = 0
init{
minCost = -1
fromNode = 0
done_flg = false
line_id = 0
}
}
 var d:Array<NODE_JCT>
init{
var i:Int
d = arrayOfNulls<NODE_JCT>(RouteUtil.MAX_JCT)
 /* ダイクストラ変数初期化 */
        		i = 0
while (i < RouteUtil.MAX_JCT)
{
d[i] = NODE_JCT()
i++
}
}
 fun setMinCost(index:Int, value:Int) {
d[index].minCost = value
}
 fun setFromNode(index:Int, value:Int) {
d[index].fromNode = value.toShort()
}
 fun setDoneFlag(index:Int, value:Boolean) {
d[index].done_flg = value
}
 fun setLineId(index:Int, value:Int) {
d[index].line_id = value.toShort()
}

 fun minCost(index:Int):Int {
return d[index].minCost
}
 fun fromNode(index:Int):Short {
return d[index].fromNode.toShort()
}
 fun doneFlag(index:Int):Boolean {
return d[index].done_flg
}
 fun lineId(index:Int):Short {
return d[index].line_id.toShort()
}
}
val dijkstra = Dijkstra()

RouteUtil.ASSERT(0 < startStationId())
 //RouteUtil.ASSERT (startStationId() != end_station_id);

        val startNode:Short
var lastNode:Short = 0
var excNode1:Short = 0
var excNode2:Short = 0
var lastNode1:Short = 0
var lastNode1_distance = 0
var lastNode2:Short = 0
var lastNode2_distance:Short = 0
var i:Int
var loopRoute:Boolean
 //int km;
        var a = 0
var b = 0
var doneNode:Short
var cost:Int
var id:Short
var lid:Short
var stationId:Short
var nLastNode:Short
var neer_node:Array<ShortArray>

 /* 途中追加か、最初からか */
        if (1 < route_list_raw.size)
{
do
{
stationId = route_list_raw[route_list_raw.size - 1].stationId
if (0 == Id2jctId(stationId.toInt()).toInt())
{
removeTail()
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
}
else
{
break
}
}
while (1 < route_list_raw.size)

stationId = route_list_raw[route_list_raw.size - 1].stationId

}
else
{
stationId = startStationId().toShort()
}

if (stationId.toInt() == end_station_id || end_station_id <= 0)
{
return 4			/* already routed */
}

if (RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_END))
{
return 5           // already finished
}
 /* ダイクストラ変数初期化 */

        startNode = Id2jctId(stationId.toInt())
lastNode = Id2jctId(end_station_id)
if (startNode.toInt() == 0)
{ /* 開始駅は非分岐駅 */
 // 駅の両隣の最寄分岐駅IDとその駅までの計算キロを得る
            neer_node = RouteUtil.GetNeerNode(stationId.toInt())
lid = LineIdFromStationId(stationId.toInt())
 // 発駅～最初の分岐駅までの計算キロを最初の分岐駅までの初期コストとして初期化
            a = Id2jctId(neer_node[0][0].toInt()).toInt()
if (!routePassed.check(a))
{
dijkstra.setMinCost(a - 1, neer_node[0][1].toInt())
dijkstra.setFromNode(a - 1, -1)	// from駅を-1(分岐駅でないので存在しない分岐駅)として初期化
dijkstra.setLineId(a - 1, lid.toInt())
}
if (neer_node[1][0].toInt() != 0)
{	// 両端あり?
b = Id2jctId(neer_node[1][0].toInt()).toInt()
if (!routePassed.check(b))
{
dijkstra.setMinCost(b - 1, neer_node[1][1].toInt())
dijkstra.setFromNode(b - 1, -1)
dijkstra.setLineId(b - 1, lid.toInt())
}
else if (routePassed.check(a))
{
System.out.printf("Autoroute:発駅の両隣の分岐駅は既に通過済み")
return -10								// >>>>>>>>>>>>>>>>>>>>>>>
}
}
else
{
 /* 盲腸線 */
                if (routePassed.check(a))
{
System.out.printf("Autoroute:盲腸線で通過済み.")
return -11								// >>>>>>>>>>>>>>>>>>>>>>>>>>
}
b = 0
}
}
else
{
dijkstra.setMinCost(startNode - 1, 0)
}

loopRoute = false

if (lastNode.toInt() == 0)
{ /* 終了駅は非分岐駅 ? */
 // 駅の両隣の最寄分岐駅IDとその駅までの計算キロを得る
            neer_node = RouteUtil.GetNeerNode(end_station_id)

 // dijkstraのあとで使用のために変数に格納
            // 終了駅の両隣の分岐駅についてはadd()でエラーとなるので不要(かどうか？）

            lastNode1 = Id2jctId(neer_node[0][0].toInt())
lastNode1_distance = neer_node[0][1].toInt()
if (neer_node[0][0].toInt() != 0)
{
if (neer_node[1][0].toInt() != 0)
{
nLastNode = 2
}
else
{
nLastNode = 1
}
}
else
{
nLastNode = 0
}
 //nLastNode = (int)neer_node.size();
            RouteUtil.ASSERT(nLastNode.toInt() == 1 || nLastNode.toInt() == 2)  /* 野辺地の大湊線はRouteUtil.ASSERT */
if (2 <= nLastNode)
{
lastNode2 = Id2jctId(neer_node[1][0].toInt())
lastNode2_distance = neer_node[1][1]
nLastNode = 2
}
else
{
 /* 着駅=閉塞線 */
                lastNode2 = 0
lastNode2_distance = 0xffff.toShort()
nLastNode = 1
}
System.out.printf("Last target=%s, %s\n", RouteUtil.StationName(Jct2id(lastNode1.toInt()).toInt()), RouteUtil.StationName(Jct2id(lastNode2.toInt()).toInt()))
}
else
{
nLastNode = 0
lastNode2 = 0
lastNode1 = lastNode2
}

if (2 == route_list_raw.size && IsSameNode(route_list_raw[route_list_raw.size - 1].lineId.toInt(),
route_list_raw[0].stationId.toInt(),
route_list_raw[route_list_raw.size - 1].stationId.toInt()))
{
id = Id2jctId(startStationId())
if (id.toInt() == 0)
{
neer_node = RouteUtil.GetNeerNode(startStationId())
if (neer_node[1][0].toInt() != 0)
{	// neer_node.size() == 2
excNode1 = Id2jctId(neer_node[0][0].toInt())		/* 渋谷 品川 代々木 */
excNode2 = Id2jctId(neer_node[1][0].toInt())		/* 渋谷 品川 新宿 */
System.out.printf("******** loopRouteX **%s, %s******\n", RouteUtil.StationName(Jct2id(excNode1.toInt()).toInt()), RouteUtil.StationName(Jct2id(excNode2.toInt()).toInt()))
loopRoute = true
}
else
{
 /* 逗子 大船 磯子 */
                }
}
else
{
excNode1 = id
excNode2 = Id2jctId(route_list_raw[route_list_raw.size - 1].stationId.toInt())
if (excNode2.toInt() == 0)
{
neer_node = RouteUtil.GetNeerNode(route_list_raw[route_list_raw.size - 1].stationId.toInt())
if (neer_node[1][0].toInt() != 0)
{	// neer_node.size() == 2
excNode1 = Id2jctId(neer_node[0][0].toInt())
excNode2 = Id2jctId(neer_node[1][0].toInt())
RouteUtil.ASSERT(id == excNode1 || id == excNode2)
loopRoute = true		/* 代々木 品川 代々木 */
}
else
{
RouteUtil.ASSERT(false)	/* 先頭で途中追加の最終ノードは分岐駅のみとしているのであり得ない */
 /* 大船 鎌倉 横須賀などは、鎌倉がremoveTail() され大船 横須賀となる*/
                    }
}
else
{
loopRoute = true
}
System.out.printf("******** loopRouteY **%s, %s******\n", RouteUtil.StationName(Jct2id(excNode1.toInt()).toInt()), RouteUtil.StationName(Jct2id(excNode2.toInt()).toInt()))
}
}

 /* dijkstra */
        while (true)
{
doneNode = -1
i = 0
while (i < RouteUtil.MAX_JCT)
{
 // ノードiが確定しているとき
                // ノードiまでの現時点での最小コストが不明の時
                if (dijkstra.doneFlag(i) || dijkstra.minCost(i) < 0)
{
i++
continue
}
 /*  確定したノード番号が-1かノードiの現時点の最小コストが小さいとき
				 *  確定ノード番号更新する
				 */
                 if (doneNode < 0 || !routePassed.check(i + 1) && dijkstra.minCost(i) < dijkstra.minCost(doneNode.toInt()))
{
doneNode = i.toShort()
}
i++
}
if (doneNode.toInt() == -1)
{
break	/* すべてのノードが確定したら終了 */
}
dijkstra.setDoneFlag(doneNode.toInt(), true)	// Enter start node

System.out.printf("[%s]", RouteUtil.StationName(Jct2id(doneNode + 1).toInt()))
if (nLastNode.toInt() == 0)
{
if (doneNode + 1 == lastNode.toInt())
{
break	/* 着ノードが完了しても終了可 */
}
}
else if (nLastNode.toInt() == 1)
{
if (doneNode + 1 == lastNode1.toInt())
{
break	/* 着ノードが完了しても終了可 */
}
}
else if (nLastNode.toInt() == 2)
{
if (dijkstra.doneFlag(lastNode1 - 1) && dijkstra.doneFlag(lastNode2 - 1))
{
break	/* 着ノードが完了しても終了可 */
}
}

val nodes = Node_next(doneNode + 1)

for (node in nodes)
{

a = node[0] - 1	// jctId

if ((!routePassed.check(a + 1) /**/ || nLastNode.toInt() == 0 && lastNode.toInt() == a + 1 ||
0 < nLastNode && lastNode1.toInt() == a + 1 ||
1 < nLastNode && lastNode2.toInt() == a + 1) /**/ && (useBulletTrain || !RouteUtil.IS_SHINKANSEN_LINE(node[2])))
{
/** コメント化しても同じだが少し対象が減るので無駄な比較がなくなる  */
					/* 新幹線でない */
                    cost = dijkstra.minCost(doneNode.toInt()) + node[1] // cost

 // ノードtoはまだ訪れていないノード
                    // またはノードtoへより小さいコストの経路だったら
                    // ノードtoの最小コストを更新
                    if ((dijkstra.minCost(a) < 0 || cost <= dijkstra.minCost(a)) && (cost != dijkstra.minCost(a) || RouteUtil.IS_SHINKANSEN_LINE(node[2])) && (!loopRoute || doneNode + 1 != excNode1.toInt() && doneNode + 1 != excNode2.toInt() || excNode1.toInt() != a + 1 && excNode2.toInt() != a + 1))
{
 /* ↑ 同一距離に2線ある場合新幹線を採用 */
                        dijkstra.setMinCost(a, cost)
dijkstra.setFromNode(a, doneNode + 1)
dijkstra.setLineId(a, node[2])
System.out.printf("+<%s(%s)>", RouteUtil.StationName(Jct2id(a + 1).toInt()), RouteUtil.LineName(dijkstra.lineId(a).toInt()))
}
else
{
System.out.printf("-<%s>", RouteUtil.StationName(Jct2id(a + 1).toInt()))
}
}
else
{
System.out.printf("x(%s)", RouteUtil.StationName(Jct2id(a + 1).toInt()))
}
}
System.out.printf("\n")
}

var route:MutableList<Int> = ArrayList()
var lineid:Short = 0
var idb:Short

if (lastNode.toInt() == 0)
{ /* 終了駅は非分岐駅 ? */
 // 最後の分岐駅の決定：
            // 2つの最後の分岐駅候補(終了駅(非分岐駅）の両隣の分岐駅)～終了駅(非分岐駅)までの
            // 計算キロ＋2つの最後の分岐駅候補までの計算キロは、
            // どちらが短いか？
            if (2 == nLastNode.toInt() && !routePassed.check(lastNode2.toInt()) && dijkstra.minCost(lastNode2 - 1) + lastNode2_distance < dijkstra.minCost(lastNode1 - 1) + lastNode1_distance)
{
id = lastNode2		// 短い方を最後の分岐駅とする
}
else
{
id = lastNode1
}
}
else
{
id = lastNode
}

System.out.printf("Last target=%s, <-- %s(%d), (%d, %d, %d)\n", RouteUtil.StationName(Jct2id(id.toInt()).toInt()), RouteUtil.StationName(Jct2id(dijkstra.fromNode(id - 1).toInt()).toInt()), dijkstra.fromNode(id - 1), lastNode.toInt(), lastNode1.toInt(), lastNode2.toInt())

 //fromNodeが全0で下のwhileループで永久ループに陥る
        if (dijkstra.fromNode(id - 1).toInt() == 0)
{
if (lastNode.toInt() == 0 && (nLastNode.toInt() == 2 && lastNode2 == startNode || lastNode1 == startNode))
{
 /* 品川―大森 */
                lid = LineIdFromStationId(end_station_id)
if (0 < InStationOnLine(lid.toInt(), stationId.toInt()))
{
System.out.printf("short-cut route.###\n")
a = add(lid.toInt(), /*stationId,*/ end_station_id)
 //                    if (0 <= a) {
 //                        route_list_cooked.clear();
 //                    }
                    lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
return a	//>>>>>>>>>>>>>>>>>>>>>>>>>
}
}
System.out.printf("can't lowcost route.###\n")
return -1002
}
 //------------------------------------------
        // 発駅(=分岐駅)でなく最初の分岐駅(-1+1=0)でない間
        // 最後の分岐駅からfromをトレース >> route[]
        while (id != startNode && 0 < id)
{
System.out.printf("  %s, %s, %s.", RouteUtil.LineName(lineid.toInt()), RouteUtil.LineName(dijkstra.lineId(id - 1).toInt()), RouteUtil.StationName(Jct2id(id.toInt()).toInt()))
if (lineid != dijkstra.lineId(id - 1))
{
if (RouteUtil.IS_SHINKANSEN_LINE(lineid.toInt()))
{
 //System.out.printf("@@@@->%d\n", lineid);
	                /* 新幹線→並行在来線 */
                    val zline = RouteUtil.GetHZLine(lineid.toInt(), Jct2id(id.toInt()).toInt())
idb = id
while (idb != startNode && dijkstra.lineId(idb - 1).toInt() == zline)
{
System.out.printf("    ? %s %s/", RouteUtil.LineName(dijkstra.lineId(idb - 1).toInt()), RouteUtil.StationName(Jct2id(idb.toInt()).toInt()))
idb = dijkstra.fromNode(idb - 1)
}
if (dijkstra.lineId(idb - 1) == lineid)
{ /* もとの新幹線に戻った ? */
 //System.out.printf(".-.-.-");
                        id = idb
continue
}
else if (idb == startNode)
{ /* 発駅？ */
if (zline == RouteUtil.GetHZLine(lineid.toInt(), Jct2id(idb.toInt()).toInt()))
{
id = idb
continue
}
 /* thru */
                        //System.out.printf("*-*-*-");
                    }
else if (idb != id)
{ /* 他路線の乗り換えた ? */
System.out.printf("%sはそうだが、%sにも新幹線停車するか?", RouteUtil.StationName(Jct2id(id.toInt()).toInt()), RouteUtil.StationName(Jct2id(idb.toInt()).toInt()))
if (zline == RouteUtil.GetHZLine(lineid.toInt(), Jct2id(idb.toInt()).toInt()))
{
id = idb
continue
}
 /* thru */
                        System.out.printf("+-+-+-: %s(%s) : ", RouteUtil.LineName(dijkstra.lineId(idb - 1).toInt()), RouteUtil.LineName(lineid.toInt()))
}
else
{
 //System.out.printf("&");
                    }
}
else
{ /* 前回新幹線でないなら */
 /* thru */
                    //System.out.printf("-=-=-=");
                }
 // 次の新幹線でも並行在来線でもない路線への分岐駅に新幹線分岐駅でない場合(金山)最初の在来線切り替えを有効にする（三河安城）
                // 新幹線に戻ってきている場合(花巻→盛岡）、花巻まで（北上から）無効化にする
                //
                route.add(id - 1)
lineid = dijkstra.lineId(id - 1)
System.out.printf("  o\n")
}
else
{
System.out.printf("  x\n")
}
id = dijkstra.fromNode(id - 1)
}

 //// 発駅=分岐駅

        System.out.printf("----------[%s]------\n", RouteUtil.StationName(Jct2id(id.toInt()).toInt()))

val route_rev = ArrayList<Int>(route.size)
var bid = -1
var ritr = route.size - 1
while (0 <= ritr)
{
System.out.printf("> %s %s\n", RouteUtil.LineName(dijkstra.lineId(route[ritr]).toInt()), RouteUtil.StationName(Jct2id(route[ritr] + 1).toInt()))
if (0 < bid && RouteUtil.IS_SHINKANSEN_LINE(dijkstra.lineId(bid).toInt()))
{
if (RouteUtil.GetHZLine(dijkstra.lineId(bid).toInt(), Jct2id(route[ritr] + 1).toInt()) == dijkstra.lineId(route[ritr]).toInt())
{
dijkstra.setLineId(route[ritr], dijkstra.lineId(bid).toInt())	/* local line -> bullet train */
route_rev.removeAt(route_rev.size - 1)
}
}
route_rev.add(route[ritr])
bid = route[ritr]
ritr--
}
 /* reverse(route_rev->route) */
        route = ArrayList(route_rev)
route_rev.clear()	/* release */

if (lastNode.toInt() == 0)
{	// RouteUtil.着駅は非分岐駅?
System.out.printf("last: %s\n", RouteUtil.LineName(dijkstra.lineId(route[route.size - 1]).toInt()))
lid = LineIdFromStationId(end_station_id) // 着駅所属路線ID
 // RouteUtil.最終分岐駅～着駅までの営業キロ、運賃計算キロを取得
            //km = Get_node_distance(lid, end_station_id, Jct2id(a));
            //km += minCost[route.get(route.size() - 1)];	// 最後の分岐駅までの累積計算キロを更新
            if (lid == dijkstra.lineId(route[route.size - 1]) || IsAbreastShinkansen(lid.toInt(), dijkstra.lineId(route[route.size - 1]).toInt(),
Jct2id(route[route.size - 1] + 1).toInt(),
end_station_id))
{
route.removeAt(route.size - 1)
 // if   着駅の最寄分岐駅の路線=最後の分岐駅?
                //      (渋谷-新宿-西国分寺-RouteUtil.国立)
                // or   平行在来線の新幹線 #141002001
                // else 渋谷-新宿-三鷹
            }
}
else
{
lid = 0
}

if (1 < route_list_raw.size && 1 < route.size && route_list_raw[route_list_raw.size - 1].lineId == dijkstra.lineId(route[0]))
{
System.out.printf("###return return!!!!!!!! back!!!!!! %s:%s#####\n", RouteUtil.LineName(route_list_raw[route_list_raw.size - 1].lineId.toInt()), RouteUtil.StationName(route_list_raw[route_list_raw.size - 1].stationId.toInt()))
removeTail()
RouteUtil.ASSERT(0 < route_list_raw.size) /* route_list_raw.size() は0か2以上 */
 //stationId = route_list_raw.get(route_list_raw.size() - 1).RouteUtil.stationId;
        }

a = 1
i = 0
while (i < route.size.toInt())
{
System.out.printf("route[] add: %s\n", RouteUtil.StationName(Jct2id(route[i] + 1).toInt()))
a = add(dijkstra.lineId(route[i]).toInt(), /*stationId,*/ Jct2id(route[i] + 1).toInt())
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
if (a <= 0 || a == 5)
{
 //RouteUtil.ASSERT (false);
                System.out.printf("####%d##%d, %d##\n", a, i, route.size)
if (a < 0 || i + 1 < route.size.toInt())
{
 //                    route_list_cooked.clear();
                    return a	/* error */
}
else
{
break
}
}
i++
 //stationId = Jct2id(route[i] + 1);
        }

 //        route_list_cooked.clear();

        if (lastNode.toInt() == 0)
{
System.out.printf("fin last:%s\n", RouteUtil.LineName(lid.toInt()))
RouteUtil.ASSERT(0 < lid)
if (a == 0)
{
 // 立川 八王子 拝島 西国立
                return -1000
}
a = add(lid.toInt(), /*stationId,*/ end_station_id)
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_JCTSP_ROUTE_CHANGE)	/* route modified */
if (a <= 0 || a == 5)
{
 //RouteUtil.ASSERT (0 == a);
                return a
}
}
return a
}


 /*  通過連絡運輸チェック
     *  param [in] line_id  路線id
     *
     *  @retval 0 = continue
     *	@retval -4 = 会社線 通過連絡運輸なし
     */
    private fun companyPassCheck(line_id:Int, stationId1:Int, stationId2:Int, num:Int):Int {

val rc:Int

if (RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_COMPNDA) || RouteUtil.IS_COMPANY_LINE(line_id) && RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_COMPNPASS))
{
return -4      /* error x a c */
}
if (RouteUtil.IS_COMPANY_LINE(line_id) && 0 == lastFlag and (1 shl RouteList.BLF_COMPNCHECK or (1 shl RouteList.BLF_COMPNPASS)))
{

lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_COMPNEND)	// if company_line

 /* pre block check d */
    		lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_COMPNCHECK)
return preCompanyPassCheck(line_id, stationId1, stationId2, num)

}
else if (!RouteUtil.IS_COMPANY_LINE(line_id) && RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_COMPNCHECK))
{

 /* after block check e f */
    		rc = postCompanyPassCheck(line_id, stationId1, stationId2, num)
if (rc == 0)
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_COMPNPASS)
lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_COMPNEND)	// if company_line
}
return rc

}
else if (RouteUtil.IS_COMPANY_LINE(line_id))
{
 /* b */
    		RouteUtil.ASSERT(!RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_COMPNDA))
RouteUtil.ASSERT(RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_COMPNCHECK))
RouteUtil.ASSERT(!RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_COMPNPASS))
RouteUtil.ASSERT(route_list_raw[route_list_raw.size - 1].lineId.toInt() != line_id)
RouteUtil.ASSERT(RouteUtil.IS_COMPANY_LINE(route_list_raw[route_list_raw.size - 1].lineId.toInt()))

lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_COMPNEND)	// if company_line

if (RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_COMPNBEGIN))
{
return 0	/* 会社線始発はとりあえず許す！ */
}

 /* 会社線乗継可否チェック(市振、目時、妙高高原、倶利伽羅) */
    		rc = companyConnectCheck(stationId1)
return rc
}
else
{
 /* g h */
    		lastFlag = RouteUtil.BIT_OFF(lastFlag, RouteList.BLF_COMPNEND)	// if company_line
return 0
}
}


 /*!
     *	前段チェック 通過連絡運輸
     *	@param [in] station_id2  add(),追加予定駅
     *	@retval 0 : エラーなし(継続)
     *	@retval -4 : 通過連絡運輸禁止
     */
    internal fun companyConnectCheck(station_id:Int):Int {
val tsql = "select pass from t_compnconc where station_id=?"
val dbo = RouteDB.db().rawQuery(tsql, arrayOf(station_id.toString()))
var r = 0	/* if disallow */
try
{
if (dbo.moveToNext())
{
r = dbo.getInt(0)
}
}

finally
{
dbo.close()
}
return if (r == 0) -4 else 0
}

 /*!
     *	前段チェック 通過連絡運輸
     *	@param [in] line_id      add(),追加予定路線
     *	@param [in] station_id1  add(),最後に追加した駅
     *	@param [in] station_id2  add(),追加予定駅
     *	@retval 0 : エラーなし(継続)
     *	@retval -4 : 通過連絡運輸禁止
     */
    internal fun preCompanyPassCheck(line_id:Int, station_id1:Int, station_id2:Int, num:Int):Int {
val cs = CompnpassSet()
var i:Int
var rc:Int

System.out.printf("Enter preCompanyPassCheck(%s, %s %s %d)\n", RouteUtil.LineName(line_id), RouteUtil.StationName(station_id1), RouteUtil.StationName(station_id2), num)

if (num <= 1)
{
 /* 会社線で始まる */
    		lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_COMPNBEGIN)
return 0
}
rc = cs.open(station_id1, station_id2)
if (rc <= 0)
{
return 0		/* Error or Non-record(always pass) as continue */
}
i = 1
while (i < num)
{
rc = cs.check(route_list_raw[i].lineId.toInt(),
route_list_raw[i - 1].stationId.toInt(),
route_list_raw[i].stationId.toInt())
if (0 <= rc)
{
break	/* OK */
}
i++
}
if (i < num)
{
return 0		/* Error or Non-record(always pass) as continue */
}
else
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_COMPNDA) /* 通過連絡運輸不正 */
return 0	/* -4 受け入れて（追加して）から弾く */
}
}

 /*	@brief 後段 通過連絡運輸チェック
     *	@param [in] line_id      add(),追加予定路線
     *	@param [in] station_id1  add(),最後に追加した駅
     *	@param [in] station_id2  add(),追加予定駅
     *	@retval 0 : エラーなし(継続)
     *	@retval -4 : 通過連絡運輸禁止
     */
    internal fun postCompanyPassCheck(line_id:Int, station_id1:Int, station_id2:Int, num:Int):Int {
val cs = CompnpassSet()
var rc:Int
var i:Int
var key1 = 0
var key2 = 0

System.out.printf("Enter postCompanyPassCheck(%s, %s %s %d)\n", RouteUtil.LineName(line_id), RouteUtil.StationName(station_id1), RouteUtil.StationName(station_id2), num)

 // 1st後段チェック	RouteUtil.ASSERT(RouteUtil.IS_COMPANY_LINE(route_list_raw.back().lineId));
    	RouteUtil.ASSERT(!RouteUtil.IS_COMPANY_LINE(line_id))

i = num - 1
while (0 < i)
{
if (key1 == 0 && RouteUtil.IS_COMPANY_LINE(route_list_raw[i].lineId.toInt()))
{
key1 = route_list_raw[i].stationId.toInt()
}
else if (key1 != 0 && !RouteUtil.IS_COMPANY_LINE(route_list_raw[i].lineId.toInt()))
{
key2 = route_list_raw[i].stationId.toInt()
break
}
i--
}
System.out.printf("  key1=%s, key2=%s\n", RouteUtil.StationName(key1), RouteUtil.StationName(key2))
if (i <= 0)
{
lastFlag = RouteUtil.BIT_ON(lastFlag, RouteList.BLF_COMPNDA) /* 通過連絡運輸不正 */
if (RouteUtil.BIT_CHK(lastFlag, RouteList.BLF_COMPNBEGIN))
{
return 0	/* 会社線始発なら終了 */
}
else
{
RouteUtil.ASSERT(false)
return -4
}
}
rc = cs.open(key1, key2)
if (rc <= 0)
{
return 0		/* Error or Non-record(always pass) as continue */
}
rc = cs.check(line_id, station_id1, station_id2)
if (rc < 0)
{
 // RouteUtil.BIT_ON(last_flag, BLF_COMPNDA); /* 通過連絡運輸不正 */
    	}
return rc	/* 0 / -4 */
}



internal class CompnpassSet {
internal inner class recordset {
 var line_id:Int = 0
 var stationId1:Int = 0
 var stationId2:Int = 0
}
 var results:Array<recordset>
 var max_record:Int = 0
 var num_of_record:Int = 0
init{
max_record = RouteUtil.MAX_COMPNPASSSET
results = arrayOfNulls<recordset>(max_record)
for (i in results.indices)
{
results[i] = recordset()
 // ウヘェ使いづれー言語
            }
num_of_record = 0
}
 /*	会社線通過連連絡運輸テーブル取得
         *	@param [in] key1   駅１
         *	@param [in] key2   駅2
         *	@return  結果数を返す（0~N, -1 Error：レコード数がオーバー(あり得ないバグ)）
         */
		 fun open(key1:Int, key2:Int):Int {
val tsql = "select en_line_id, en_station_id1, en_station_id2" +
" from t_compnpass" +
" where station_id1=? and station_id2=?"
var i:Int
val dbo = RouteDB.db().rawQuery(tsql, arrayOf(key1.toString(), key2.toString()))
var rc = -1
try
{
i = 0
while (dbo.moveToNext())
{
if (max_record <= i)
{
RouteUtil.ASSERT(false)
return -1		/* too many record */
}
results[i].line_id = dbo.getInt(0)
results[i].stationId1 = dbo.getInt(1)
results[i].stationId2 = dbo.getInt(2)
i++
}
num_of_record = i
rc = i	/* num of receord */
}

finally
{
dbo.close()
}
return rc	/* db error */
}

 /*!
         *	@brief 通過連絡運輸チェック
         *
         *	@param [in] line_id      add(),追加予定路線
         *	@param [in] station_id1  add(),最後に追加した駅
         *	@param [in] station_id2  add(),追加予定駅
         *	@retval 0 : エラーなし(継続)
         *	@retval -4 : 通過連絡運輸禁止
         */
		 fun check(line_id:Int, station_id1:Int, station_id2:Int):Int {
var i:Int
if (num_of_record <= 0)
{
return 0
}
i = 0
while (i < num_of_record)
{
if (results[i].line_id and 0x80000000.toInt() != 0)
{
 /* company */
        			if (RouteUtil.BIT_CHK(results[i].line_id, RouteUtil.CompanyIdFromStation(station_id2)))
{
return 0	/* OK possible pass */
}

if (RouteUtil.BIT_CHK(results[i].line_id, RouteUtil.CompanyAnotherIdFromStation(station_id2)) && RouteUtil.BIT_CHK(results[i].line_id, RouteUtil.CompanyIdFromStation(station_id1)))
{
return 0	/* OK possible pass */
}

}
else if (results[i].line_id == line_id)
{
if (results[i].stationId1 == 0 || 0 < RouteUtil.InStation(station_id1, line_id, results[i].stationId1, results[i].stationId2) && 0 < RouteUtil.InStation(station_id2, line_id, results[i].stationId1, results[i].stationId2))
{
return 0	/* OK possible to pass */
}
}
else if (results[i].line_id == 0)
{
break	/* can't possoble */
}
i++
}
return -4
}

private fun en_line_id(index:Int):Int {
return results[RouteUtil.Limit(index, 0, RouteUtil.MAX_COMPNPASSSET - 1)].line_id
}
private fun en_station_id1(index:Int):Int {
return results[RouteUtil.Limit(index, 0, RouteUtil.MAX_COMPNPASSSET - 1)].stationId1
}
private fun en_station_id2(index:Int):Int {
return results[RouteUtil.Limit(index, 0, RouteUtil.MAX_COMPNPASSSET - 1)].stationId2
}

} // CompnpassSet

companion object {

 //Static
    //	@brief 分岐特例の分岐路線a+乗換駅dから本線bと分岐駅cを得る
    //
    //	@param [in]  jctLineId         a 分岐路線
    //	@param [in]  transferStationId d 乗換駅
    //	@param [out] jctspdt   b 本線, c 分岐駅
    //
    //	@return type 0: usual, 1-3:type B
    //
    internal fun RetrieveJunctionSpecific(jctLineId:Int, transferStationId:Int, jctspdt:JCTSP_DATA):Int {
val tsql =
 //"select calc_km>>16, calc_km&65535, (lflg>>16)&32767, lflg&32767 from t_lines where (lflg&(1<<31))!=0 and line_id=?1 and station_id=?2"; +
                //	"select type,jctsp_line_id1, jctsp_station_id1, jctsp_line_id2, jctsp_station_id2" +
                //	" from t_jctspcl where id=(" +
                //	"	select calc_km from t_lines where (lflg&(1<<31))!=0 and line_id=?1 and station_id=?2)"; +
                "select type,jctsp_line_id1, jctsp_station_id1, jctsp_line_id2, jctsp_station_id2" +
" from t_jctspcl where id=(" +
"	select lflg&255 from t_lines where (lflg&((1<<31)|(1<<29)))!=0 and line_id=?1 and station_id=?2)"
var type = 0

val dbo = RouteDB.db().rawQuery(tsql, arrayOf(jctLineId.toString(), transferStationId.toString()))
try
{
if (dbo.moveToNext())
{
type = dbo.getInt(0)
jctspdt.jctSpMainLineId = dbo.getInt(1)
jctspdt.jctSpStationId = dbo.getInt(2)
jctspdt.jctSpMainLineId2 = dbo.getInt(3)
jctspdt.jctSpStationId2 = dbo.getInt(4)
}
}

finally
{
dbo.close()
}
RouteUtil.ASSERT(jctspdt.jctSpMainLineId2 == 0 && jctspdt.jctSpStationId2 == 0 || jctspdt.jctSpMainLineId2 != 0 && jctspdt.jctSpStationId2 != 0)
if (jctspdt.jctSpStationId2 == 0)
{	// safety
jctspdt.jctSpMainLineId2 = 0
}
return type
}

 //	@brief 分岐特例の分岐路線a+乗換駅dから本線bと分岐駅cを得る
    //
    //	@param [in]  jctLineId         a 分岐路線
    //	@param [in]  transferStationId d 乗換駅
    //	@param [out] jctSpMainLineId   b 本線
    //	@param [out] jctSpStationId    c 分岐駅
    //
    //	@return type 0: usual, 1-3:type B
    //
    internal fun GetBsrjctSpType(line_id:Int, station_id:Int):Int {
val tsql = "select type from t_jctspcl where id=(select lflg&255 from t_lines where line_id=?1 and station_id=?2)"
var type = -1
val dbo = RouteDB.db().rawQuery(tsql, arrayOf(line_id.toString(), station_id.toString()))
try
{
if (dbo.moveToNext())
{
type = dbo.getInt(0)
}
}

finally
{
dbo.close()
}
return type
}


 /*	経路追加
    	 *
    	 *	@param [in] line_id      路線ident
    	 *	@param [in] stationId1   駅1 ident
    	 *	@param [in] stationId2   駅2 ident
    	 *	@param [in] ctlflg       デフォルト0(All Off)
    	 *							 bit8: 新幹線乗換チェックしない
    	 *
         *  @retval 0 = OK(last)         ADDRC_LAST
         *  @retval 1 = OK               ADDRC_OK
         *  @retval 4 = OK(last-company) ADDRC_CEND
         *  //@retval 2 = OK(re-route)
         *  @retval 5 = already finished ADDRC_END)
         *  @retval -1 = overpass(復乗エラー)
         *  @retval -2 = 経路エラー(stationId1 or stationId2はline_id内にない)
         *  @retval -3 = operation error(開始駅未設定)
         *	@retval -4 = 会社線 通過連絡運輸なし
         *  @retval -100 DB error
         *	@retval last_flag bit4-0 : reserve
         *	@retval last_flag bit5=1 : 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
         *	@retval last_flag bit5=0 : 次にremoveTailでlastItemの通過マスクをOffする(typeOでもPでもないので)
         *	@retval last_flag bit6=1 : 分岐特例区間指定による経路変更あり
    	 */
    internal val ADD_BULLET_NC = 1 shl 8



 //static
    // 駅ID(分岐駅)の最寄りの分岐駅を得る(全路線）
    // [jct_id, calc_km, line_id][N] = f(jct_id)
    //
    //	@param [in] jctId   分岐駅
    //	@return 分岐駅[0]、計算キロ[1]、路線[2]
    //
    private fun Node_next(jctId:Int):List<Array<Int>> {
val tsql = "select case jct_id when ?1 then neer_id else jct_id end as node, cost, line_id" +
" from t_node" +
" where jct_id=?1 or neer_id=?1 order by node"

val result = ArrayList<Array<Int>>()

val dbo = RouteDB.db().rawQuery(tsql, arrayOf(jctId.toString()))
try
{
while (dbo.moveToNext())
{
val r1 = arrayOfNulls<Int>(3)
r1[0] = dbo.getInt(0)	// jct_id
r1[1] = dbo.getInt(1)	// cost(calc_km)
r1[2] = dbo.getInt(2)	// line_id
result.add(r1)
}
}

finally
{
dbo.close()
}
return result
}

 // static
    //	駅ID→分岐ID
    //
    private fun Id2jctId(stationId:Int):Short {
val ctx = RouteDB.db().rawQuery(
"select id from t_jct where station_id=?", arrayOf(stationId.toString()))
var rc:Short = 0 // if error
try
{
if (ctx.moveToNext())
{
rc = ctx.getShort(0)
}
}

finally
{
ctx.close()
}
return rc
}
 // static
    //	分岐ID→駅ID
    //
    private fun Jct2id(jctId:Int):Short {
val ctx = RouteDB.db().rawQuery(
"select station_id from t_jct where rowid=?", arrayOf(jctId.toString()))
var rc:Short = 0   // if error
try
{
if (ctx.moveToNext())
{
rc = ctx.getShort(0)
}
}

finally
{
ctx.close()
}
return rc
}



 //static
    //	分岐ID→駅名
    //
    private fun JctName(jctId:Int):String {
var name = ""		//[MAX_STATION_CHR];

val ctx = RouteDB.db().rawQuery(
"select name from t_jct j left join t_station t on j.station_id=t.rowid where id=?",
arrayOf(jctId.toString()))
try
{
if (ctx.moveToNext())
{
name = ctx.getString(0)
}
}

finally
{
ctx.close()
}
return name
}


 //static
    //	駅は路線内にあるか否か？
    //
    private fun InStationOnLine(line_id:Int, station_id:Int):Int {
val ctx = RouteDB.db().rawQuery(
 //		"select count(*) from t_lines where line_id=?1 and station_id=?2");
                "select count(*) from t_lines where (lflg&((1<<31)|(1<<17)))=0 and line_id=?1 and station_id=?2",
arrayOf(line_id.toString(), station_id.toString()))
var rc = 0
try
{
if (ctx.moveToNext())
{
rc = ctx.getInt(0)
}
}

finally
{
ctx.close()
}
return rc
}


 // 駅(station_id)の所属する路線IDを得る.
    // 駅は非分岐駅で, 路線は1つしかないと解っていることを前提とする.
    //
    // line_id = f(station_id)
    //
    private fun LineIdFromStationId(station_id:Int):Short {
val ctx = RouteDB.db().rawQuery(
"select line_id" +
" from t_lines" +
" where station_id=?" +
" and 0=(lflg&((1<<31)|(1<<17)))", arrayOf(station_id.toString()))
var rc:Short = 0
try
{
if (ctx.moveToNext())
{
rc = ctx.getShort(0)
}
}

finally
{
ctx.close()
}
return rc
}


 // 駅(station_id)の所属する路線IDを得る.
    // 駅は非分岐駅で, 路線は1つしかないと解っていることを前提とする.
    //
    // line_id = f(station_id)
    //
    private fun LineIdFromStationId2(station_id1:Int, station_id2:Int):Short {
val ctx = RouteDB.db().rawQuery(
"select line_id from t_lines where station_id=?1 and 0=(lflg&((1<<31)|(1<<17))) and line_id in " + "(select line_id from t_lines where station_id=?2 and 0=(lflg&((1<<31)|(1<<17))))",
arrayOf(station_id1.toString(), station_id2.toString()))
var rc:Short = 0
try
{
if (ctx.moveToNext())
{
rc = ctx.getShort(0)
}
}

finally
{
ctx.close()
}
return rc
}


 //static
    //	二つの駅は、同一ノード内にあるか
    //
    //	@param [in] line_id    line
    //	@param [in] station_id1  station_id 1
    //	@param [in] station_id2  station_id 2
    //	@return true : 同一ノード
    //	@return fase : 同一ノードではない
    //
    private fun IsSameNode(line_id:Int, station_id1:Int, station_id2:Int):Boolean {
val tsql = " select count(*)" +
" from t_lines" +
" where line_id=?1" +
" and (lflg&((1<<31)|(1<<17)|(1<<15)))=(1<<15)" +
" and sales_km>" +
" 		(select min(sales_km)" +
" 		from t_lines" +
" 		where line_id=?1" +
" 		and (station_id=?2 or" +
" 			 station_id=?3))" +
" and sales_km<" +
" 		(select max(sales_km)" +
" 		from t_lines" +
" 		where line_id=?1" +
" 		and (station_id=?2 or" +
" 			 station_id=?3))"

val dbo = RouteDB.db().rawQuery(tsql, arrayOf(line_id.toString(), station_id1.toString(), station_id2.toString()))
var same = false
try
{
if (dbo.moveToNext())
{
same = dbo.getInt(0) <= 0
}
}

finally
{
dbo.close()
}
return same
}

 //static
    //	路線の駅1から駅2方向の最初の分岐駅
    //  (neerestで使おうと思ったが不要になった-残念)
    //	@param [in] line_id    line
    //	@param [in] station_id1  station_id 1(from)
    //	@param [in] station_id2  station_id 2(to)
    //	@return station_id : first function(contains to station_id1 or station_id2)
    //
    private fun NeerJunction(line_id:Int, station_id1:Int, station_id2:Int):Int {
val tsql = "select station_id from t_lines where line_id=?1 and" +
" case when" +
" (select sales_km from t_lines where line_id=?1 and station_id=?3)<" +
" (select sales_km from t_lines where line_id=?1 and station_id=?2) then" +
" sales_km=(select max(sales_km) from t_lines where line_id=?1 and (lflg&((1<<17)|(1<<31)))=0 and" +
" sales_km<=(select sales_km from t_lines where line_id=?1 and station_id=?2) and (lflg&((1<<17)|(1<<31)|(1<<15)))=(1<<15))" +
" else" +
" sales_km=(select min(sales_km) from t_lines where line_id=?1 and (lflg&((1<<17)|(1<<31)))=0 and " +
" sales_km>=(select sales_km from t_lines where line_id=?1 and station_id=?3) and (lflg&((1<<17)|(1<<31)|(1<<15)))=(1<<15))" +
" end;"

val dbo = RouteDB.db().rawQuery(tsql, arrayOf(line_id.toString(), station_id1.toString(), station_id2.toString()))
var stationId = 0
try
{
if (dbo.moveToNext())
{
stationId = dbo.getInt(0)
}
}

finally
{
dbo.close()
}
return stationId
}

 //static
    //	運賃計算キロを返す(最短距離算出用)
    //
    //	@param [in] line_id     路線
    //	@param [in] station_id1 発
    //	@param [in] station_id2 至
    //
    //	@retuen 営業キロ or 計算キロ
    //
    private fun Get_node_distance(line_id:Int, station_id1:Int, station_id2:Int):Int {
val tsql = "select case when sum(calc_km)=0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end" +
" from	t_lines" +
" where line_id=?1 and (lflg&(1<<31))=0 and	(station_id=?2 or station_id=?3)"

/****	// 140416
          * "select case when sum(calc_km)=0 then max(sales_km)-min(sales_km) else max(calc_km)-min(calc_km) end"
          * " from	t_lines"
          * " where line_id=?1"
          * " and	(lflg&(1<<31))=0"
          * " and	sales_km>=(select min(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))"
          * " and	sales_km<=(select max(sales_km) from t_lines where line_id=?1 and (station_id=?2 or station_id=?3))";

          * /* old // if 0
          * "select case when (l1.calc_km-l2.calc_km)=0 then l1.sales_km - l2.sales_km else  l1.calc_km - l2.calc_km end"
          * " from t_lines l1"
          * " left join t_lines l2"
          * " where l1.line_id=?1"
          * " and l2.line_id=?1"
          * " and l1.sales_km>l2.sales_km"
          * " and ((l1.station_id=?2 and l2.station_id=?3)"
          * " or (l1.station_id=?3 and l2.station_id=?2))";
          */
        val dbo = RouteDB.db().rawQuery(tsql, arrayOf<String>((line_id).toString(), (station_id1).toString(), (station_id2).toString()))
var rc = 0
try
{
if (dbo.moveToNext())
{
rc = dbo.getInt(0)
}
}

finally
{
dbo.close()
}
return rc
}

 //static
    //	路線は新幹線の在来線か?
    //
    //	@param [in] line_id1		比較元路線(在来線)
    //	@param [in] line_id2		路線(新幹線)
    //	@param [in] station_id1		駅1 (発) 在来線接続駅であること
    //	@param [in] station_id2		駅2 (着) 在来線接続駅でなくても可
    //
    //	@return	true 並行在来線
    //
    internal fun IsAbreastShinkansen(line_id1:Int, line_id2:Int, station_id1:Int, station_id2:Int):Boolean {
val i:Int
val w:Int

if (!RouteUtil.IS_SHINKANSEN_LINE(line_id2))
{
return false
}
val hzl = RouteUtil.EnumHZLine(line_id2, station_id1, station_id2)
if (hzl.size < 3)
{
if (hzl.size < 1)
{
RouteUtil.ASSERT(false)
}
else
{
if (hzl.get(0) === line_id1.toInt())
{
return 0 < InStationOnLine(line_id1, station_id2)
}
}
return false
}
i = 0
while (i < hzl.size.toInt())
{
if (0x10000 < hzl.get(i))
{
w = 0xffff and hzl.get(i)
}
else
{
w = hzl.get(i)
}
if (w == line_id1)
{
return true
}
else if (0 != w)
{
return false
}
i++
}
return false
}




 //static
    //	新幹線乗換可否のチェック
    //	add() =>
    //
    //	@param [in] line_id1  前路線
    //	@param [in] line_id2  今路線
    //	@param [in] station_id1  前回接続(発)駅
    //	@param [in] station_id2  接続駅
    //	@param [in] station_id3  着駅
    //
    //	@return true: OK / false: NG
    //
    //                 国府津 s1                東京
    // l1 東海道線     小田原 s2 東海道新幹線   静岡
    // l2 東海道新幹線 名古屋 s3 東海道線       草薙
    //
    private fun CheckTransferShinkansen(line_id1:Int, line_id2:Int, station_id1:Int, station_id2:Int, station_id3:Int):Boolean {
val bullet_line:Int
val local_line:Int
val dir:RouteUtil.LINE_DIR
val hzl:Int

if (RouteUtil.IS_SHINKANSEN_LINE(line_id2))
{
bullet_line = line_id2		// 在来線->新幹線乗換
local_line = line_id1
hzl = RouteUtil.GetHZLine(bullet_line, station_id2, station_id3)

}
else if (RouteUtil.IS_SHINKANSEN_LINE(line_id1))
{
bullet_line = line_id1		// 新幹線->在来線乗換
local_line = line_id2
hzl = RouteUtil.GetHZLine(bullet_line, station_id2, station_id1)

}
else
{
return true				// それ以外は対象外
}
if (local_line != hzl)
{
return true
}

 // table.A
        dir = RouteUtil.DirLine(line_id1, station_id1, station_id2)
if (dir == RouteUtil.DirLine(line_id2, station_id2, station_id3))
{
return true		// 上り→上り or 下り→下り
}
if (dir == RouteUtil.LINE_DIR.LDIR_ASC)
{	// 下り→上り
return (((RouteUtil.AttrOfStationOnLineLine(local_line, station_id2).ushr(19)) and 0x01) != 0)
}
else
{				// 上り→下り
return (((RouteUtil.AttrOfStationOnLineLine(local_line, station_id2).ushr(19)) and 0x02) != 0)
}
 //return true;
    }
}
 /*
//	経路内を着駅で終了する(以降の経路はキャンセル)
//
//	@param [in] stationId   駅ident
//
void terminate(int stationId) {
    int i;
    int stationIdFrom = 0;
    int newLastIndex = 0x7fffffff;
    int line_id = -1;
    //int stationIdTo;

    for (i = 0; i < route_list_raw.size(); i++) {
        if (stationIdFrom != 0) {
            // stationIdはroute_list_raw[i].lineId内のstationIdFromから
            //              route_list_raw[i].statonIdの間にあるか?
            if (0 != RouteUtil.InStation(stationId, route_list_raw.get(i).lineId, stationIdFrom, route_list_raw.get(i).stationId)) {
                newLastIndex = i;
                line_id = route_list_raw.get(i).lineId;
                //stationIdTo = route_list_raw[i].stationId;
                break;
            }
        } else {
            RouteUtil.ASSERT (i == 0);
        }
        stationIdFrom = route_list_raw.get(i).stationId;
    }
    if (newLastIndex < route_list_raw.size()) {
        while (newLastIndex < route_list_raw.size()) {
            removeTail();
        }
        RouteUtil.ASSERT ((newLastIndex<=1) || route_list_raw.get(newLastIndex - 1).stationId == stationIdFrom);
        add(line_id, stationId);
        end_station_id = (short)stationId;
    }
}


*/
}/* --------------------------------------- */// operator=(final Route& source_route)
 //	経路の末尾を除去
 //
 //	@param [in] begin_off    最終ノードの始点もOffするか(デフォルト:Offしない)
 //
 //	経路設定中 経路重複発生時
 //	経路設定キャンセル
 //
 //	@param [in]  bWithStart  開始駅もクリアするか(デフォルトクリア)
 //	@note
 //	x stop_jctId > 0 : 指定分岐駅前までクリア(指定分岐駅含まず)
 //	x stop_jctId = 0 : (default)指定分岐駅指定なし
 //	x stop_jctId < 0 : 開始地点も削除
 //
 /* =true */ // Route
