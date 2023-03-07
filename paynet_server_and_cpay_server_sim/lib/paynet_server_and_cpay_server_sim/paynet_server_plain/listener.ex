defmodule PaynetServerAndCpayServerSim.PaynetServerPlain.Listener do
  def child_spec(opts) do
    #:ranch.child_spec(:paynet_listener, :ranch_ssl, opts, PaynetServerAndCpayServerSim.PaynetServer.Handler, [name: :paynet])
    :ranch.child_spec(:paynet_listener_plain, :ranch_tcp, opts, PaynetServerAndCpayServerSim.PaynetServer.Handler, [name: :paynet_plain])
  end
end
