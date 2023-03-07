defmodule PaynetServerAndCpayServerSim.Cpay_Server.Supervisor do
  use Supervisor

  def start_link(args) do
    Supervisor.start_link(__MODULE__, args)
  end

  @impl true
  def init(_abc) do
    children = [
        {PaynetServerAndCpayServerSim.CpayServer.Listener, [port: 17501]}
    ]

    Supervisor.init(children, strategy: :one_for_one)
  end
end
