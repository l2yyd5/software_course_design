import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import installElementPlus from './plugins/element'
import store from './store'
import Emitter from 'tiny-emitter'

const app = createApp(App).use(store)
app.config.globalProperties.Emitter = new Emitter()
installElementPlus(app)
app.use(router).mount('#app')