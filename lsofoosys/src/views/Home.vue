<template>
  <div class="common-layout">
  <el-container>
    <el-aside width="188px">
      <div class="el-aside-top">
        <img :src="logo" class="sidebar-logo">
        <h1 class="sidebar-title">{{ status }} </h1>
      </div>
      <div class="el-aside-down">
        <el-scrollbar>
          <el-menu
            :uniqueOpened="true"
            :default-active="$route.path"
            :class="el-menu-vertical-demo"
            background-color="#48444c"
            text-color="#d4d4d4"
            active-text-color="#fff"
            active-background-color="#061220"
            router>
            <el-menu-item index="/General">
              <template #title>General</template>
            </el-menu-item>
            <el-menu-item index="/ProcessInfo">
              <template #title>Process Info</template>
            </el-menu-item>
            <el-menu-item index="/MessageBox">
              <template #title>MessageBox Hook</template>
            </el-menu-item>
            <el-menu-item index="/Heap">
              <template #title>Heap Hook</template>
            </el-menu-item>
            <el-menu-item index="/File">
              <template #title>File Hook</template>
            </el-menu-item>
            <el-menu-item index="/Regedit">
              <template #title>Regedit Hook</template>
            </el-menu-item>
            <el-menu-item index="/Net">
              <template #title>Net Analysis</template>
            </el-menu-item>
            <el-menu-item index="/Memory">
              <template #title>Memory Analysis</template>
            </el-menu-item>
            <el-menu-item index="/Warning">
              <template #title>Warning</template>
            </el-menu-item>
          </el-menu>
        </el-scrollbar>
      </div>
    </el-aside>
    <router-view/>
  </el-container>
</div>
</template>

<script>
export default {
  data(){
    return {
      title: 'Software behavior analysis system',
      logo: require('@/assets/v_logo.png'),
      status: 'inactive'
    }
  },
  methods: {
    giveStatus: function () {
      setInterval(() => {
        this.Emitter.emit('homeStatus', this.status)
      }, 1000)
    }
  },
  created() {
    this.Emitter.on('changea', (a) => {
      this.status = a
    })
    this.giveStatus()
  }
}
</script>

<style>
  .el-header {
    background-color: #302c3c;
    text-align: center;
    --el-header-height: 100px !important;
    line-height: 100px;
    margin-bottom: 5px;
  }
  
  .el-footer {
    background-color: #302c3c;
    text-align: center;
    height: 60px;
    line-height: 60px;
  }

  .el-aside {
    border-radius: 20px;
    text-align: center;
    height: 745px;
  }

  .el-main {
    background-color: #302c3c;
    text-align: center;
    height: 580px;
  }

  .el-aside-top {
    background-color: #48444c;

    font-family:Arial,Helvetica,sans-serif;
    font-style: normal;
    font-weight:bold;
    font-size: 2rem;
    color: #ffffff;

    text-align: center;
    line-height: 100px;
    height: 100px;
    margin-bottom: 5px;
  }

  .el-aside-down {
    background-color: #48444c;
    text-align: center;
    height: 640px;
  }

  .el-menu-item {
    font-size: 17px !important;
  }

  .el-menu-item.is-active {
    background-color: #302c3c !important;
  }

  body > .el-container {
    height: 100%;
  }
  
.sidebar-logo {
  width: 32px;
  height: 32px;
  vertical-align: middle;
  margin-right: 12px;
}

.sidebar-title {
  display: inline-block;
  margin: 0;
  color: #fff;
  
  font-style: normal;
  font-weight:bold;
  line-height: 50px;
  font-size: 24px;
  font-family: Avenir, Helvetica Neue, Arial, Helvetica, sans-serif;
  vertical-align: middle;
}
</style>