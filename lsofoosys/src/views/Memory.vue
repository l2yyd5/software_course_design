<template>
  <el-container>
    <el-header style="font-size:32px;color:white">Memory Hook</el-header>
    <el-main>
        <div class="app-container">
            <el-tree
                :default-expanded-keys="expandedList7"
                node-key="id"
                :data="data7"
                @node-expand="nodeExpand"
                @node-collapse="nodeCollapse"
                :props="defaultProps"
                :default-expand-all="false"
            />

        </div>
    </el-main>
    <el-footer>
      <el-button type="clear" v-on:click="clearData()" round>Clear Information</el-button>
    </el-footer>
  </el-container>
</template>

<script>
import axios from 'axios';
export default {
  data() {
    return {
      Gstatus: 'inactive',
      expandedList7:[],
      data7: [
        {
          "id": "0",
          "label": "Memcpy",
          "children": []
        },
      ],
      defaultProps: {
        children: 'children',
        label: 'label'
      }
    }
  },  

  methods: {
    getData(){
      setInterval(()=>{
        if(this.Gstatus === 'active'){
          axios.get("http://127.0.0.1:23333/MemAction").then((res) =>{
            this.data7 = res.data.data
          }).catch(err => {
            console.log(err)
          })
        }
      }, 1000)
    },
    clearData(){
      for(var i=0; i<this.data7.length;i++){
        this.data7[i]['children'] = []
      }
      this.expandedList7 = []
      axios.get("http://127.0.0.1:23333/MemAction?action=cleardata").then((res) =>{
          console.log(res)
        }).catch(err => {
          console.log(err)
        });
    },
    nodeExpand(data) {
        this.expandedList7.push(data.id)
    },
    nodeCollapse(data) {
      this.expandedList7.splice(this.expandedList7.indexOf(data.id), 1);
    },
  },

  created: function () {
    this.Emitter.on('homeStatus', (a) => {
      if(a != this.Gstatus){
        console.log(a)
        this.Gstatus = a
      }
    })
    this.getData()
  }
}
</script>

<style>
  .el-button--clear{
    padding: 1rem;
    background-color: #3aa1cc;
    color:#fbfdfd;

    border-radius: .3rem;
    width: 80%;
    height: 10%;
  }
  .el-tree {
    background-color: #48444c;

    font-family:Arial,Helvetica,sans-serif;
    font-style: normal;
    font-size: 2rem;
    color: #ffffff;

    text-align: center;
  }

  .el-tree-node.is-current > .el-tree-node__content {
    background-color: #302c3c !important;
    color: white;
  }
 
  .el-checkbox__input.is-checked+.el-checkbox__label {
    color: black;
  }

  .el-tree-node > .el-tree-node__content{
    background-color :#606068 !important;
    color: #fffefd;
  }
</style>