defmodule PaynetServerAndCpayServerSim.PaynetServerPlain.Supervisor do
  use Supervisor

  def start_link(args) do
    Supervisor.start_link(__MODULE__, args)
  end

  @impl true
  def init(_abc) do
    children = [
        {PaynetServerAndCpayServerSim.PaynetServerPlain.Listener,
          [port: 17503]}
    ]

    Supervisor.init(children, strategy: :one_for_one)
  end

  def handle_info({:send, msg}, _state) do
    IO.inspect "received message:"
    IO.inspect msg
  end
end


#{PaynetServerAndCpayServerSim.PaynetServer.Listener,
#          [port: 17502, verify: :verify_peer,
#          keyfile: 'C:\\Users\\haimi.yahya\\Documents\\work_repos\\certstrap\\out\\zombieserver.key',
#          certfile: 'C:\\Users\\haimi.yahya\\Documents\\work_repos\\certstrap\\out\\zombieserver.crt',
#          cacertfile: 'C:\\Users\\haimi.yahya\\Documents\\work_repos\\certstrap\\out\\Zombie_Spotters_Ltd.crt',
#          password: '1234',
#          secure_renegotiate: true,
#          reuse_sessions: true
#          ]}
