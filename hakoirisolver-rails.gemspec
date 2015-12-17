# coding: utf-8
lib = File.expand_path('../lib', __FILE__)
$LOAD_PATH.unshift(lib) unless $LOAD_PATH.include?(lib)
require 'hakoirisolver/rails/version'

Gem::Specification.new do |spec|
  spec.name          = "hakoirisolver-rails"
  spec.version       = Hakoirisolver::Rails::VERSION
  spec.authors       = ["maromaro0013"]
  spec.email         = ["ota0120@gmail.com"]

  spec.summary       = %q{Immutable Linked List implemented in C-Extensions}
  spec.description   = %q{Immutable Linked List implemented in C-Extensions}
  spec.homepage      = "TODO: Put your gem's website or public repo URL here."
  spec.license       = "MIT"
  spec.extensions    = %w[ext/hakoirisolver/extconf.rb]

  spec.files         = `git ls-files -z`.split("\x0").reject { |f| f.match(%r{^(test|spec|features)/}) }
  spec.require_paths = ["lib"]

  spec.add_development_dependency "bundler", "~> 1.10"
  spec.add_development_dependency "rake", "~> 10.0"
end
