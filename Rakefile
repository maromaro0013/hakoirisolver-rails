require "bundler/gem_tasks"
require "rspec/core/rake_task"
require "rake/extensiontask"

Rake::ExtensionTask.new "hakoirisolver" do |ext|
  ext.lib_dir = "lib/hakoirisolver"
end

RSpec::Core::RakeTask.new(:spec)

task :default => :spec
