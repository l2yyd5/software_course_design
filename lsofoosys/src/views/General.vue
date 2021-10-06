<template>    
  <el-container>
    <el-header style="font-size:32px;color:white">
      Software behavior analysis system v 0.0.1</el-header>
    <el-main>
      <div align="center" style="margin-top: 100px">
        <el-input id="input01"
          type="textarea"
          :autosize="{ minRows: 5, maxRows: 5}"
          placeholder="  "
          v-model="textarea">
        </el-input>
      </div>
      <el-button type="primary" v-on:click="openFile()" round>选择文件</el-button>

      <input type="file" name="filename" id="open01" @change="changefile()" style="display:none" />
    </el-main>
    <el-footer>
      <div class="div_allinline">
        <div class="subdiv_allinline_left">
          <el-button type="greenC" :disabled="Gstatus==='active'" v-on:click="startHook()" round>start</el-button>
        </div>
        <div class="subdiv_allinline_right">
          <el-button type="greenC" :disabled="Gstatus==='inactive'" v-on:click="stopHook()" round>stop</el-button>
        </div>
      </div>
    </el-footer>
  </el-container>
</template>

<script>
import axios from "axios"
export default {
  data() {
    return {
      Gstatus: 'inactive',
      file: ''
    }
  },

  methods: {
    changefile() {
      this.file = document.getElementById('open01').files[0].path
      document.getElementById('input01').value = document.getElementById('open01').files[0].path
    },
    openFile: function () {
      document.getElementById('open01').click()
    },
    startHook: function () {
      if(this.file != ''){
        var api = "http://127.0.0.1:23333/GeneralAction?action=start&file=" + this.file
        axios.get(api).then((res) =>{
          console.log(res.data)
          if(res.data.status == "success"){
            this.Emitter.emit('changea', 'active')
            this.Gstatus = 'active'
          }
        }).catch(err => {
          console.log(err)
            this.Emitter.emit('changea', 'inactive')
            this.Gstatus = 'inactive'
        });
      }
    },
    stopHook: function () {
      var api = "http://127.0.0.1:23333/GeneralAction?action=stop"
      axios.get(api).then((res) =>{
        console.log(res.data)
      }).catch(err => {
        console.log(err)
      });
      this.Emitter.emit('changea', 'inactive')
      this.Gstatus = 'inactive'
    },
  },
  created: function (){
    this.Emitter.on('homeStatus', (a) => {
      if(a != this.Gstatus){
        console.log(a)
        this.Gstatus = a
      }
    })
  }
}
</script>

<style>

.el-button--primary {
  padding: 1rem;

  color: white;
  background-color: #2EA169;

  border-radius: .3rem;

  margin-top: 100px;
  width: 100%;
  height: 10%;

  text-align: center;
  font-weight: bold;
}
.el-button--greenC {
  padding: 1rem;

  color: white;
  background-color: #2EA169;

  border-radius: .3rem;

  width: 100%;
  height: 10%;

  text-align: center;
  font-weight: bold;
}

.div_allinline
{
    text-align:center;
    margin:0 auto;
    padding:0;
    clear:both;
}

.subdiv_allinline_left
{
  margin-right: 4rem!important;
  width: 20%;
  padding:0;
  display:inline-block;
  _display:block;
  *display:block;
  /* zoom:1; */
}

.subdiv_allinline_right
{
  margin-left: 4rem!important;
  width: 20%;
  padding:0;
  display:inline-block;
  _display:block;
  *display:block;
  /* zoom:1; */
}

</style>