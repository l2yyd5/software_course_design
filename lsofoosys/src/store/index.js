import { createStore } from 'vuex'

// const net = require('net')


export default createStore({
  state: {
    status:"inactive"
  },
  mutations: {
    getStatus(state){
      return state.status
    },
    toActive(state){
      state.status = "active"
    },
    toInactive(state){
      state.status = "inactive"
    }
  },
  actions: {
  },
  modules: {
  }
})
