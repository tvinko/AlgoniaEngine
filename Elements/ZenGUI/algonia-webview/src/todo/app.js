function invokeMe()
    {
      external.invoke(JSON.stringify({cmd : 'error', msg : document.getElementById("txtSymbol").value}));
    }