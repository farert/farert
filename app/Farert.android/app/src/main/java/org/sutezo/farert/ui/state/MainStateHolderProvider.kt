package org.sutezo.farert.ui.state

import androidx.lifecycle.ViewModelProvider
import androidx.lifecycle.ViewModelStoreOwner

object MainStateHolderProvider {
    private var instance: MainStateHolder? = null
    
    fun getInstance(owner: ViewModelStoreOwner): MainStateHolder {
        if (instance == null) {
            instance = ViewModelProvider(owner)[MainStateHolder::class.java]
        }
        return instance!!
    }
    
    fun clear() {
        instance = null
    }
}