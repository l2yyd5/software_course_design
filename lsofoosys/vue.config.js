module.exports = {
    pluginOptions: {
      electronBuilder: {
        // List native deps here if they don't work
        externals: ['my-native-dep'],
        // If you are using Yarn Workspaces, you may have multiple node_modules folders
        // List them all here so that VCP Electron Builder can find them
        nodeModulesPath: ['../../node_modules', './node_modules'],

        builderOptions: {
          "appId": "com.lsofoosys.app",
          "productName":"lsofoosys",//项目名，也是生成的安装文件名，即aDemo.exe
          "copyright":"Copyright © 2021",//版权信息
          "directories":{
              "output":"./dist"//输出文件路径
          },
          "win":{//win相关配置
            "icon":"./public/telegram-soft.png",//图标，当前图标在根目录下，注意这里有两个坑
            "target": [
              {
                "target": "nsis",//利用nsis制作安装程序
                "arch": [
                    "x64"
                ]
              }
            ]
          },
          "nsis": {
              "oneClick": false, // 是否一键安装
              "allowElevation": true, // 允许请求提升。 如果为false，则用户必须使用提升的权限重新启动安装程序。
              "allowToChangeInstallationDirectory": true, // 允许修改安装目录
              "createDesktopShortcut": true, // 创建桌面图标
              "createStartMenuShortcut": true,// 创建开始菜单图标
          }
        }
      }
    }
  }