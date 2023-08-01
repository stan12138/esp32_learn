<template>
  <v-container class="">
      <v-responsive class="align-center text-center fill-height">
          <v-card class="mx-auto" max-width="600">
              <v-toolbar flat dense>
                  <v-toolbar-title>
                      <span class="text-subheading">Monitor</span>
                  </v-toolbar-title>
                  <v-spacer></v-spacer>
                  <v-btn variant="text" :color="monitor_color" size="x-large" @click="monitor_click">{{ monitor_text }}</v-btn>
              </v-toolbar>
              <v-container fluid>
              <v-row dense>
                  <v-col v-for="card in cards" :key="card.title" :cols="card.flex">
                  <v-card>
                      <canvas class="monitor" :id="card.id"></canvas>
                  </v-card>
                  </v-col>
              </v-row>
              </v-container>
          </v-card>
          <v-card class="mx-auto" max-width="600" style="margin-top: 20px;">
              <v-toolbar flat dense>
                  <v-toolbar-title>
                      <span class="">Change Speed</span>
                  </v-toolbar-title>
              </v-toolbar>
              <v-card-text>
                  <v-row class="mb-4" justify="space-between">
                      <v-col class="text-left">
                          <span class="text-h2 font-weight-light" v-text="bpm"></span>
                          <span class="subheading font-weight-light me-1"></span>
                      </v-col>
                  </v-row>

                  <v-slider v-model="bpm"  track-color="grey" min="0" max="1" :step="0.01" @update:model-value="post_data">
                      <template v-slot:prepend>
                          <v-btn size="small" variant="text" @click="decrement">-</v-btn>
                      </template>
                      <template v-slot:append>
                          <v-btn size="small" variant="text" @click="increment">+</v-btn>
                      </template>
                  </v-slider>
              </v-card-text>
          </v-card>
      </v-responsive>
  </v-container>
</template>

<script>

export default {
  name:'motor',
  data: () => ({
      bpm: 0.5,
      monitor_text: "Start",
      monitor_color: "green",
      monitor_stop: true,
      interValId: null,
      ctx_speed: null,
      aim_line: [],
      speed_line: [],
      speed_axis: {
          xlim: {min: 0, max: 100},
          ylim: {min: -1, max: 1},
          width: 0,
          height: 0,
          x_trans: {offset: 0, factor: 0},
          y_trans: {offset: 0, factor: 0}
      },
      ctx_pwm: null,
      pwm_axis: {
          ylim: {min: -1, max: 1},
          xlim: {min: 0, max: 100},
          width: 0,
          height: 0,
          x_trans: {offset: 0, factor: 0},
          y_trans: {offset: 0, factor: 0}
      },
      pwm_line: [],
      ctx_error: null,
      error_axis: {
          ylim: {min: -1, max: 1},
          xlim: {min: 0, max: 100},
          width: 0,
          height: 0,
          x_trans: {offset: 0, factor: 0},
          y_trans: {offset: 0, factor: 0}
      },
      error_line: [],
      cards: [
          { title: 'Speed', flex: 12, id: "Speed" },
          { title: 'Error', flex: 6 , id: "Error"},
          { title: 'PWM', flex: 6 , id: "PWM"},
      ],
      t: 0
  }),
  computed: {
      animationDuration () {
          return `${60 / this.bpm}s`
      },
  },
  methods: {
      decrement () {
          this.bpm--
      },
      increment () {
          this.bpm++
      },
      toggle () {
          this.isPlaying = !this.isPlaying
      },
      monitor_click: function() {
          this.monitor_stop =!this.monitor_stop;
          if(!this.monitor_stop){
              this.monitor_text = "Stop";
              this.monitor_color = "red";
          } else {
              this.monitor_text = "Start"
              this.monitor_color = "green";
          }
          if(!this.monitor_stop) {
              this.interValId = setInterval(this.draw, 100);
              //this.draw();
          } else {
              clearInterval(this.interValId);
              // this.draw();
          }
      },
      draw: function(){
          if(!this.ctx_speed) {
              const canvas = document.getElementById('Speed');
              this.speed_axis.height = canvas.clientHeight;
              this.speed_axis.width = canvas.clientWidth;
              canvas.width = this.speed_axis.width;
              canvas.height = this.speed_axis.height;
              this.cal_axis_trans_params(this.speed_axis);
              this.ctx_speed = canvas.getContext("2d");
              this.ctx_speed.textBaseline = "middle";
          }
          if(!this.pwm_speed) {
              const canvas = document.getElementById('PWM');
              this.pwm_axis.height = canvas.clientHeight;
              this.pwm_axis.width = canvas.clientWidth;
              canvas.width = canvas.clientWidth;
              canvas.height = canvas.clientHeight;
              this.cal_axis_trans_params(this.pwm_axis);
              this.ctx_pwm = canvas.getContext("2d");
              this.ctx_pwm.textBaseline = "middle";
          }
          if(!this.error_speed) {
              const canvas = document.getElementById('Error');
              this.error_axis.height = canvas.clientHeight;
              this.error_axis.width = canvas.clientWidth;
              canvas.width = canvas.clientWidth;
              canvas.height = canvas.clientHeight;
              this.cal_axis_trans_params(this.error_axis);
              this.ctx_error = canvas.getContext("2d");
              this.ctx_error.textBaseline = "middle";
          }
          this.get_data();

      },
      begin_draw(motor_data) {
                  // 设置speed图
          if(this.speed_line.length > (this.speed_axis.width-1)) {
              this.speed_line = this.speed_line.slice(1, this.speed_axis.width);
              this.aim_line = this.aim_line.slice(1, this.speed_axis.width);
          }
          this.speed_line.push(motor_data.speed);
          this.aim_line.push(motor_data.aim);
          this.change_ylim(this.speed_line, this.speed_axis);
          this.draw_line([this.speed_line, this.aim_line], this.ctx_speed, this.speed_axis);
          // 设置error图
          if(this.error_line.length > (this.error_axis.width-1)) {
              this.error_line = this.error_line.slice(1, this.error_axis.width);
          }
          this.error_line.push(motor_data.error);
          this.change_ylim(this.error_line, this.error_axis);
          this.draw_line([this.error_line,], this.ctx_error, this.error_axis);
          // 设置PWM图
          if(this.pwm_line.length > (this.pwm_axis.width-1)) {
              this.pwm_line = this.pwm_line.slice(1, this.pwm_axis.width);
          }
          this.pwm_line.push(motor_data.pwm);
          this.change_ylim(this.pwm_line, this.pwm_axis);
          this.draw_line([this.pwm_line,], this.ctx_pwm, this.pwm_axis);
      },
      change_ylim(line, axis_info) {
          if(line.length < 5) {
              return ;
          }
          let max_value = Math.max(...line);
          let min_value = Math.min(...line);
          
          let space = (max_value - min_value)*0.2;
          let y_min = min_value - space;
          let y_max = max_value + space;
          //console.log(max_value, min_value, space, y_min, y_max, axis_info);
          this.set_ylim(y_min, y_max, axis_info);
      },
      cal_axis_trans_params: function(axis_info) {
          // let width = axis_info.xlim.max - axis_info.xlim.min;
          // axis_info.x_trans.factor = axis_info.width/width;

          let height = axis_info.ylim.max - axis_info.ylim.min;
          axis_info.y_trans.factor = axis_info.height/height;
      },
      axis_trans: function(y, axis_info) {
          // let a = axis_info.x_trans.factor*(x - axis_info.xlim.min)
          let b = axis_info.y_trans.factor*(y - axis_info.ylim.min)
          return axis_info.height-b
      },
      // set_xlim: function(min, max, axis_info) {
      //     axis_info.xlim.max = max;
      //     axis_info.xlim.min = min;
      //     this.cal_axis_trans_params(axis_info);
      // },
      set_ylim: function(min, max, axis_info) {
          axis_info.ylim.max = max;
          axis_info.ylim.min = min;
          this.cal_axis_trans_params(axis_info);
      },
      draw_line: function(line_list, ctx, axis_info) {
          ctx.clearRect(0,0,axis_info.width,axis_info.height);
          this.draw_axis(ctx, axis_info);
          let color = ["blue", "red"];
          let color_index = 0;
          for(let line_index in line_list) {
              let line = line_list[line_index];
              //console.log(line, line_list)
              ctx.beginPath();
              ctx.strokeStyle = color[color_index];
              let p = this.axis_trans(line[0], axis_info)
              ctx.moveTo(0, p);
              for(let index=1; index<line.length; index++) {
                  p = this.axis_trans(line[index], axis_info)
                  ctx.lineTo(index-1, p);
              }
              ctx.stroke();
              ctx.closePath();
              color_index ++;
          }

      },
      draw_axis: function(ctx, axis_info) {
          ctx.beginPath();
          ctx.strokeStyle = "black";
          ctx.moveTo(20, axis_info.height);
          ctx.lineTo(20, 10);
          ctx.lineTo(15, 20);
          ctx.moveTo(20, 10);
          ctx.lineTo(25, 20);
          let space = (axis_info.ylim.max - axis_info.ylim.min)/10;
          for(let index = 1; index < 10; index++) {
              let y = index*space+axis_info.ylim.min;
              let p = this.axis_trans(y, axis_info);
              ctx.moveTo(20,p);
              ctx.lineTo(25,p);
              ctx.fillText(y.toFixed(2), 28, p);
          }
          ctx.stroke();

          ctx.closePath();
      },
      get_data() {
        let myHeaders = new Headers({
            'Access-Control-Allow-Origin': '*',
            'Content-Type': 'text/plain'
        });
        let url = "/api/v1/pid/info"

        fetch(url).then(response => response.json()).then(result => {this.begin_draw(result)})
      },
      async post_data(value) {
        let data = {"v": value}
        let url = "/api/v1/pid/change_aim"
        const response = await fetch(url, {
            method: 'POST', 
            mode: 'cors',
            cache: 'no-cache',
            credentials: 'same-origin', 
            headers: {
                'Content-Type': 'application/json'
            },
            redirect: 'follow',
            referrerPolicy: 'no-referrer', 
            body: JSON.stringify(data) 
          });
          return response;
      }


}
}
</script>

<style scoped>
@keyframes metronome-example {
  from {
    transform: scale(.5);
  }

  to {
    transform: scale(1);
  }
}

.v-avatar--metronome {
  animation-name: metronome-example;
  animation-iteration-count: infinite;
  animation-direction: alternate;
}
.monitor {
  width: 100%;
  height: 1000%;
  border-radius: 6px;
  background: #FFF;
  /* box-shadow: -3px -3px 15px 0px rgba(0, 0, 0, 0.25), 3px 3px 10px 0px rgba(0, 0, 0, 0.25); */
}
</style>
