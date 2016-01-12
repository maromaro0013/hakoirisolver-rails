require "json"
require "./hakoirisolver.o"

stage0 = JSON.parse '{"framesize":{"w":4,"h":5},"endpoint":{"x":3,"y":5},"panels":[{"x":0,"y":0,"w":1,"h":2,"type":"common"},{"x":1,"y":0,"w":2,"h":2,"type":"target"},{"x":3,"y":0,"w":1,"h":2,"type":"common"},{"x":0,"y":2,"w":1,"h":2,"type":"common"},{"x":1,"y":2,"w":1,"h":1,"type":"common"},{"x":2,"y":2,"w":1,"h":1,"type":"common"},{"x":3,"y":2,"w":1,"h":2,"type":"common"},{"x":1,"y":3,"w":1,"h":1,"type":"common"},{"x":2,"y":3,"w":1,"h":1,"type":"common"},{"x":0,"y":4,"w":1,"h":1,"type":"common"},{"x":3,"y":4,"w":1,"h":1,"type":"common"}]}'
stage1 = JSON.parse '{"framesize":{"w":4,"h":5},"endpoint":{"x":3,"y":5},"panels":[{"x":0,"y":0,"w":1,"h":1,"type":"common"},{"x":1,"y":0,"w":2,"h":2,"type":"target"},{"x":3,"y":0,"w":1,"h":1,"type":"common"},{"x":0,"y":1,"w":1,"h":1,"type":"common"},{"x":3,"y":1,"w":1,"h":1,"type":"common"},{"x":0,"y":2,"w":2,"h":1,"type":"common"},{"x":2,"y":2,"w":2,"h":1,"type":"common"},{"x":0,"y":3,"w":2,"h":1,"type":"common"},{"x":2,"y":3,"w":2,"h":1,"type":"common"},{"x":0,"y":4,"w":1,"h":1,"type":"common"},{"x":3,"y":4,"w":1,"h":1,"type":"common"}]}'
stage2 = JSON.parse '{"framesize":{"w":4,"h":5},"endpoint":{"x":3,"y":5},"panels":[{"x":0,"y":0,"w":1,"h":2,"type":"common"},{"x":1,"y":0,"w":2,"h":2,"type":"target"},{"x":3,"y":0,"w":1,"h":2,"type":"common"},{"x":0,"y":2,"w":1,"h":1,"type":"common"},{"x":1,"y":2,"w":1,"h":1,"type":"common"},{"x":2,"y":2,"w":1,"h":1,"type":"common"},{"x":3,"y":2,"w":1,"h":1,"type":"common"},{"x":0,"y":3,"w":2,"h":1,"type":"common"},{"x":2,"y":3,"w":2,"h":1,"type":"common"},{"x":0,"y":4,"w":1,"h":1,"type":"common"},{"x":3,"y":4,"w":1,"h":1,"type":"common"}]}';

stage = stage2

solver = HakoiriSolver.new
solver.set_field_info stage["framesize"]["w"].to_i, stage["framesize"]["h"].to_i, stage["endpoint"]["x"].to_i, stage["endpoint"]["y"].to_i

for panel in stage["panels"] do
  type = "0"
  case panel["type"]
  when "common"
    type = "0"
  when "target"
    type = "1"
  end
  solver.add_panel_to_field panel["x"].to_i, panel["y"].to_i, panel["w"].to_i, panel["h"].to_i, type.to_i
end

solver.init_solver
ret = solver.data_validate

solver.solve_field

message = ""
move_cnt = 0
move_actions = []

while (message = solver.pop_message) != "" do
  m = message.to_s.split ","
  move_actions[move_cnt] = {}
  move_actions[move_cnt]["index"] = m[0]
  move_actions[move_cnt]["action"] = m[1]
  move_cnt += 1
end

puts move_actions.to_s

solver.delete_solver
