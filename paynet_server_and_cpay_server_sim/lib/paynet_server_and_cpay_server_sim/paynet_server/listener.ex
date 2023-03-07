defmodule PaynetServerAndCpayServerSim.PaynetServer.Listener do
  def child_spec(opts) do
    #:ranch.child_spec(:paynet_listener, :ranch_ssl, opts, PaynetServerAndCpayServerSim.PaynetServer.Handler, [name: :paynet])
    :ranch.child_spec(:paynet_listener, :ranch_ssl, opts, PaynetServerAndCpayServerSim.PaynetServer.Handler, [name: :paynet])
  end
end
