package router

import (
	"../dataProcessing"
	"github.com/gin-gonic/gin"
	"net/http"
)

func InfoHandler(DataInfo *[]dataProcessing.Level1) gin.HandlerFunc {
	return func(c *gin.Context){
		action := c.DefaultQuery("action", "getdata")
		if action == "getdata"{
			//fmt.Println(*DataInfo)
			c.JSONP(http.StatusOK, gin.H{"data": *DataInfo})
		}else if action == "cleardata"{
			for i := range *DataInfo{
				(*DataInfo)[i].Children1 = (*DataInfo)[i].Children1[:0]
			}
		}
	}
}