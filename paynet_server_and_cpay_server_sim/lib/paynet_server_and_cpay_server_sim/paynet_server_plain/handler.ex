defmodule PaynetServerAndCpayServerSim.PaynetServerPlain.Handler do
  use GenServer

  @behaviour :ranch_protocol
  @timeout 120000
  @send_interval 1

  @impl true
  def start_link(ref, transport, opts) do
    {:ok, :proc_lib.spawn_link(__MODULE__, :init, [{ref, transport, opts}])}
  end

  #def send_msg(pid, data) do
    #GenServer.cast(pid, {:push, element})
   # send(pid, {:send, data})
  #end

  @impl true
  def init({ref, transport, _opts}) do

    :timer.sleep(100)

    {:ok, socket} = :ranch.handshake(ref)

    IO.inspect "client connected"

    #Process.send_after(self(), :ikan, @send_interval)
    #Process.send_after(self(), :kill_self, 5*1000)

    :ok = transport.setopts(socket, active: true, mode: :binary, packet: 0)
    :gen_server.enter_loop(__MODULE__, [], {socket, transport}, @timeout)
  end

  @impl true
  def handle_info({:tcp, socket, data}, {socket, transport} = state) do

    <<_header::size(16), data::binary>> = data

    IO.inspect "paynet server received:"
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

  @impl true
  def handle_info({:ssl,socket, data}, {socket, _transport} = state) do

    parse_msg(data)

    {:noreply, state}
  end

  def handle_info({:tcp_closed, _}, state) do

    IO.inspect "client disconnected"
    {:noreply, state, @timeout}
  end

  def handle_info({:ssl_closed, _}, state) do

    IO.inspect "client disconnected"
    {:noreply, state, @timeout}
  end

  def handle_info({:tcp_error, _, _}, state) do

    IO.inspect "client disconnected"
    {:noreply, state, @timeout}
  end

  def handle_info(:timeout, _socket) do
    IO.inspect "connection timeout"
  end

  def handle_info(:ikan, {sock, :ranch_tcp} = state) do

    data = "0200ping"
    msg_size = byte_size(data)
    msg = <<msg_size::size(16)>> <> data

    :gen_tcp.send(sock, msg)

    Process.send_after(self(), :ikan, @send_interval)
    {:noreply, state}
  end

  def handle_info(:ikan, {sock, :ranch_ssl} = state) do

    data = "0200ping"
    msg_size = byte_size(data)
    msg = <<msg_size::size(16)>> <> data

    IO.inspect("forming msg header:" <> Integer.to_string(msg_size))

    :ssl.send(sock, msg)

    Process.send_after(self(), :ikan, @send_interval)
    {:noreply, state}
  end

  def handle_info(:kill_self, state) do

    :erlang.exit(self(), :kill)


    {:noreply, state}
  end

  def process_request(data) do

    case data do
      "0200" <> other ->  "0210" <> other
      "0210" <> _ -> ""
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

  defp parse_msg(data) do
    <<header_size::size(16), tail::binary>> = data
    <<data_val::binary-size(header_size), tail::binary>> = tail

    "0200ping" <> counter = data_val
    IO.inspect(counter)

    case byte_size(tail) do
      0 -> nil
      _ -> parse_msg(tail)
    end
  end
end
