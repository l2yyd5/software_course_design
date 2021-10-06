import { createRouter, createWebHashHistory } from 'vue-router'
import Home from '../views/Home.vue'
// import Layout from '@/layout'

const routes = [
  {
    path: '/',
    name: 'Home',
    component: Home,
    redirect: '/General',
    children: [
    {
      path: '/General',
      name: 'General',
      component: () => import('../views/General.vue'),
      hidden: true
    },
    {
      path: '/ProcessInfo',
      name: 'ProcessInfo',
      component: () => import('../views/ProcessInfo.vue'),
      hidden: true
    },
    {
      path: '/MessageBox',
      name: 'MessageBox',
      component: () => import('../views/MessageBox.vue'),
      hidden: true
    },
    {
      path: '/Heap',
      name: 'Heap',
      component: () => import('../views/Heap.vue'),
      hidden: true
    },
    {
      path: '/File',
      name: 'File',
      component: () => import('../views/File.vue'),
      hidden: true
    },
    {
      path: '/Regedit',
      name: 'Regedit',
      component: () => import('../views/Regedit.vue'),
      hidden: true
    },
    {
      path: '/Net',
      name: 'Net',
      component: () => import('../views/Net.vue'),
      hidden: true
    },
    {
      path: '/Memory',
      name: 'Memory',
      component: () => import('../views/Memory.vue'),
      hidden: true
    },
    {
      path: '/Warning',
      name: 'Warning',
      component: () => import('../views/Warning.vue'),
      hidden: true
    }]
  },

  {
    path: '/404',
    component: () => import('@/views/404')
  },
  { path: '/:pathMatch(.*)*', redirect: '/404'}
]

const router = createRouter({
  history: createWebHashHistory(),
  routes
})

export default router
