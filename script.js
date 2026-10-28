const painel = document.getElementById("chatPainel");
const conversa = document.getElementById("conversa");
const formularioInicial = document.getElementById("perguntaInicialForm");
const perguntaInicial = document.getElementById("perguntaInicial");
const formularioMensagem = document.getElementById("formularioMensagem");
const mensagem = document.getElementById("mensagem");
const minimizarChat = document.getElementById("minimizarChat");
const restaurarChat = document.getElementById("restaurarChat");

function normalizar(texto) {
  return texto
    .toLocaleLowerCase("pt-BR")
    .normalize("NFD")
    .replace(/[\u0300-\u036f]/g, "");
}

function adicionarMensagem(texto, classe) {
  const item = document.createElement("div");
  item.className = `message ${classe}`;
  item.textContent = texto;
  conversa.appendChild(item);
  conversa.scrollTop = conversa.scrollHeight;
}

function procurar(pergunta) {
  let resultado = null;
  let melhorPontuacao = 0;
  const perguntaNormalizada = normalizar(pergunta);

  for (const procedimento of window.procedimentos || []) {
    let pontuacao = 0;

    for (const palavra of procedimento.palavrasChave || []) {
      const termo = normalizar(palavra);

      if (perguntaNormalizada.includes(termo)) {
        pontuacao += termo.split(" ").length;
      }
    }

    if (pontuacao > melhorPontuacao) {
      resultado = procedimento;
      melhorPontuacao = pontuacao;
    }
  }

  return resultado;
}

function responder(pergunta) {
  adicionarMensagem(pergunta, "user");

  const procedimento = procurar(pergunta);

  if (!procedimento) {
    adicionarMensagem(
      "Não encontrei essa orientação na base demonstrativa. Tente perguntar usando outras palavras.",
      "assistant"
    );
    return;
  }

  const passos = procedimento.resposta.passos
    .map((passo, indice) => `${indice + 1}. ${passo}`)
    .join("\n");

  const resposta = [
    procedimento.resposta.introducao,
    procedimento.titulo,
    passos,
    `Observação: ${procedimento.resposta.observacao}`,
    `Fonte: ${procedimento.fonte}`
  ].join("\n\n");

  adicionarMensagem(resposta, "assistant");
}

function abrirChat(pergunta) {
  painel.classList.remove("is-hidden");
  painel.setAttribute("aria-hidden", "false");
  formularioInicial.classList.add("is-hidden");
  restaurarChat.classList.add("is-hidden");

  if (pergunta) {
    responder(pergunta);
  }

  mensagem.focus();
}

formularioInicial.addEventListener("submit", (evento) => {
  evento.preventDefault();

  const pergunta = perguntaInicial.value.trim();
  if (!pergunta) return;

  perguntaInicial.value = "";
  abrirChat(pergunta);
});

formularioMensagem.addEventListener("submit", (evento) => {
  evento.preventDefault();

  const pergunta = mensagem.value.trim();
  if (!pergunta) return;

  mensagem.value = "";
  responder(pergunta);
});

minimizarChat.addEventListener("click", () => {
  painel.classList.add("is-hidden");
  painel.setAttribute("aria-hidden", "true");
  formularioInicial.classList.remove("is-hidden");
  restaurarChat.classList.remove("is-hidden");
});

restaurarChat.addEventListener("click", () => {
  abrirChat();
});


