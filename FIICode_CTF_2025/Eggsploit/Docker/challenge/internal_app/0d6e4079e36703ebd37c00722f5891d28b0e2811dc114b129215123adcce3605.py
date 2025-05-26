from flask import Flask, request, render_template_string

app = Flask(__name__)
FLAG = "Part3: _only_at_c0c0ric0}"
app.config['FLAG'] = "Part3: _only_at_c0c0ric0}"

AUTH_TOKEN = "supersecrettoken123"

template_s = '''
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Internal Document Viewer</title>
    <link href="https://cdn.jsdelivr.net/npm/ninja-ui/dist/ninja.min.css" rel="stylesheet">
</head>
<body class="p-6 bg-gray-100">
    <div class="max-w-3xl mx-auto bg-white shadow rounded-lg p-6">
        <h1 class="text-xl font-semibold mb-4 text-gray-800">Internal Document Viewer</h1>
        <form method="GET" action="/document" class="space-y-3">
            <div>
                <label class="block text-sm font-medium">Document ID (Note: the las part of the flag is here, FLAG)</label>
                <input type="text" name="id" class="n-input w-full" placeholder="e.g. report_2024">
            </div>
            <button class="n-btn n-btn-primary">Fetch</button>
        </form>

        {% if error %}
            <div class="n-alert n-alert-error mt-4">{{ error }}</div>
        {% endif %}
            <div class="n-card mt-4">
                <div class="n-card-body">
                    <pre class="whitespace-pre-wrap text-sm">
'''

template_f = '''</pre>
                </div>
            </div>
    </div>
</body>
</html>
'''

@app.route('/document')
def document():
    # Check for Authorization
    auth_header = request.headers.get("Authorization", "")
    if auth_header != f"{AUTH_TOKEN}":
        return "401 Unauthorized", 401

    doc_id = request.args.get("id", "")
    content = ''
    error = ''

    if not doc_id:
        error = "Missing document ID"
        doc_id=''
    
    return render_template_string(template_s+doc_id+template_f, content=content, error=error)

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=6379, debug=True)