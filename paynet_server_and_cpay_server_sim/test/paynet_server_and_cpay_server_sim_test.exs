defmodule PaynetServerAndCpayServerSimTest do
  use ExUnit.Case
  doctest PaynetServerAndCpayServerSim

  test "greets the world" do
    assert PaynetServerAndCpayServerSim.hello() == :world
  end
end
