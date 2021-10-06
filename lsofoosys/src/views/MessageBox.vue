<template>
  <el-container>
    <el-header style="font-size:32px;color:white">MessageBox Hook</el-header>
    <el-main>
        <div class="app-container">
            <el-tree type="test"
                :default-expanded-keys="expandedList2"
                node-key="id"
                :data="data2"
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
      Gstatus:'inactive',
      expandedList2:[],
      data2: [
        {
          "id": "0",
          "label": "MessageBoxW",
          "children": []
        },
        {
          "id": "1",
          "label": "MessageBoxA",
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
          axios.get("http://127.0.0.1:23333/MessageBoxAction").then((res) =>{
            this.data2 = res.data.data;
          }).catch(err => {
            console.log(err)
          })
        }
      }, 1000)
    },
    clearData(){
      for(var i=0; i<this.data2.length;i++){
        this.data2[i]['children'] = []
      }
      this.expandedList2 = []
      axios.get("http://127.0.0.1:23333/MessageBoxAction?action=cleardata").then((res) =>{
          console.log(res)
        }).catch(err => {
          console.log(err)
        });
    },
    nodeExpand(data) {
        this.expandedList2.push(data.id)
    },
    nodeCollapse(data) {
      this.expandedList2.splice(this.expandedList2.indexOf(data.id), 1);
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