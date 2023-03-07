defmodule PaynetServerAndCpayServerSim.CpayServer.Handler do
  use GenServer

  @behaviour :ranch_protocol
  @timeout 120000
  @send_interval 30*1000

  @impl true
  def start_link(ref, transport, opts) do
    {:ok, :proc_lib.spawn_link(__MODULE__, :init, [{ref, transport, opts}])}
  end

  @impl true
  def init({ref, transport, _opts}) do
    {:ok, socket} = :ranch.handshake(ref)

    IO.inspect "client connected"
    IO.inspect(ref)
    #Process.send_after(:cpay_listener, :ayam, 1000)

    counter = 1

    Process.send_after(self(), :ikan, @send_interval)

    :ok = transport.setopts(socket, active: :once)
    :gen_server.enter_loop(__MODULE__, [], {socket, transport, counter}, @timeout)
  end

  @impl true
  def handle_info({:tcp, socket, data}, {socket, transport, _counter} = state) do

    <<_header::size(16), data::binary>> = data

    IO.inspect "cpay server received:"
    IO.inspect data

    resp = process_request(data)

    data = resp
    msg_size = byte_size(data)
    msg = <<msg_size::size(16)>> <> data

    case resp do
      "" -> nil
      _ -> case transport.send(socket, msg) do
          :ok -> nil
          {:error, :closed} -> IO.inspect "client disconnected"
        end
    end

    :ok = transport.setopts(socket, active: :once)
    {:noreply, state, @timeout}
  end

  def handle_info({:tcp_closed, _}, state) do

    IO.inspect "client disconnected"
    {:noreply, state, @timeout}
  end

  def handle_info({:tcp_error, _, _}, state) do

    IO.inspect "client disconnected"
    {:noreply, state, @timeout}
  end

  def handle_info({:error, :closed}, state) do

    IO.inspect "client disconnected"
    {:noreply, state}
  end

  @impl true
  def handle_info({:ssl,socket, data}, {socket, _transport, _counter} = state) do

    IO.inspect "received: " <> data

    {:noreply, state}
  end

  def handle_info(:ikan, {sock, transport, counter} = state) do

    #IO.inspect("ikan..")

    data = "0200ping" <> Integer.to_string(counter)
    msg_size = byte_size(data)
    msg = <<msg_size::size(16)>> <> data

    #IO.inspect("msg_size:")
    #IO.inspect(msg_size)
    #IO.inspect(data)

    #IO.inspect "sending.." <> data

    :gen_tcp.send(sock, msg)

    Process.send_after(self(), :ikan, @send_interval)
    {:noreply, {sock, transport, counter + 1}}
  end

  def handle_info(:ayam, state) do
    IO.inspect "muah"
    {:noreply, state}
  end

  def process_request(data) do

    case data do
      "0210" <> _ -> ""
      "0200" <> other ->  "0210" <> other
      "<11B" <> _ -> "<2D#EMFK.V(Working Key)#1234#>"
      "<99#" <> _ -> cond do
						String.contains?(data, "#1111 1111#>") -> "<NNNNNNNNNNNN>"
						true -> "<YYYYYYYYYYYY>"
					 end
      "<98#" <> _ -> "<99#8888888888888888#00000000#000>"
      "<31#" <> _ -> "<99#0#Y#>"
      _ -> data
    end
  end



end
