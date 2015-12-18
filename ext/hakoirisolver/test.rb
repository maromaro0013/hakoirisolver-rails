require "json"
require "./hakoirisolver.o"

stage0 = JSON.parse '{"framesize":{"w":4,"h":5},"endpoint":{"x":3,"y":5},"panels":[{"x":0,"y":0,"w":1,"h":2,"type":"common"},{"x":1,"y":0,"w":2,"h":2,"type":"target"},{"x":3,"y":0,"w":1,"h":2,"type":"common"},{"x":0,"y":2,"w":1,"h":2,"type":"common"},{"x":1,"y":2,"w":1,"h":1,"type":"common"},{"x":2,"y":2,"w":1,"h":1,"type":"common"},{"x":3,"y":2,"w":1,"h":2,"type":"common"},{"x":1,"y":3,"w":1,"h":1,"type":"common"},{"x":2,"y":3,"w":1,"h":1,"type":"common"},{"x":0,"y":4,"w":1,"h":1,"type":"common"},{"x":3,"y":4,"w":1,"h":1,"type":"common"}]}'
#puts stage0.to_s
#puts stage0["framesize"].to_s

solver = HakoiriSolver.new
solver.set_field_info stage0["framesize"]["w"].to_i, stage0["framesize"]["h"].to_i, stage0["endpoint"]["x"].to_i, stage0["endpoint"]["y"].to_i

for panel in stage0["panels"] do
  type = "0"
  case panel["type"]
  when "common"
    type = "0"
  when "target"
    type = "1"
  end
#puts panel["w"]
  solver.add_panel_to_field panel["x"].to_i, panel["y"].to_i, panel["w"].to_i, panel["h"].to_i, type.to_i
end

#puts solver.get_panel_count.to_s
#solver.add_panel_to_field 0, 0, 1, 1, 0

#puts solver.get_panel_count.to_s

solver.init_solver
ret = solver.data_validate
puts ret.to_s
solver.solve_field
solver.delete_solver
