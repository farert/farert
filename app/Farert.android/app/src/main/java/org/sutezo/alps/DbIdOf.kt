package org.sutezo.alps

import org.sutezo.alps.farertAssert.*


object DbIdOf {

    val retrieve_id_map = HashMap<String, Int>()

    init {
        var company_line_align_id = 300
        val dbo = RouteDB.db().rawQuery("select cline_align_id from t_global limit(1)", null)
        try {
            if (dbo.moveToNext()) {
                company_line_align_id = dbo.getInt(0)
            }
        } finally {
            dbo.close()
        }
        retrieve_id_map["company_line_align_id"] = company_line_align_id
    }

    fun company_line_align_id() = retrieve_id_map["company_line_align_id"] ?: 300

    fun station(name: String) : Int {
        if (retrieve_id_map.containsKey(name)) {
            retrieve_id_map[name]?.let {
                return it
            }
        }
        val id = RouteUtil.GetStationId(name)
        retrieve_id_map[name] = id
        return id
    }

    fun line(name: String) : Int {
        if (retrieve_id_map.containsKey(name)) {
            retrieve_id_map[name]?.let {
                return it
            }
        }
        val id = RouteUtil.GetLineId(name)
        retrieve_id_map[name] = id
        return id
    }
}