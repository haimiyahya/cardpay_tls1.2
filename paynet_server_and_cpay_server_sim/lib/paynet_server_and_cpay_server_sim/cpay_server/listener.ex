defmodule PaynetServerAndCpayServerSim.CpayServer.Listener do
  @spec child_spec(any) :: %{
          id: {:ranch_embedded_sup, any},
          start: {:ranch_embedded_sup, :start_link, [...]},
          type: :supervisor
        }
  def child_spec(opts) do
    :ranch.child_spec(:cpay_listener, :ranch_tcp, opts, PaynetServerAndCpayServerSim.CpayServer.Handler, [])
  end
end
