package org.sutezo.alps

import org.sutezo.alps.farertAssert.*


object DbIdOf {

    val retrieve_id_map = HashMap<String, Int>()

    init {

    }

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