from flask import *
import subprocess
import json

app = Flask(__name__)

data = {
    'commands': {
        'date': {'methods': 'GET'},
        'list': {'methods': 'POST'},
        'download': {'methods': 'POST'}
    }
}

@app.get('/')
def root():
    return Response(status=200,
                    content_type='application/json',
                    response=json.dumps(data)
                    )

@app.get('/date')
def getDate():
    command = ['date']
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    output = {'STDOUT': result.stdout, 'STDERR': result.stderr}

    return Response(status=200,
                    content_type='application/json',
                    response=json.dumps(output)
                    )

@app.post('/list')
def listFiles():
    if request.is_json:
        data = request.get_json()

        # Check if 'parameters' is provided in the JSON request
        if 'parameters' not in data:
            return jsonify({
                'error': 'Missing parameters',
                'message': 'Please provide the "parameters" field in the request body.'
            }), 400  # HTTP Status Code for Bad Request

        # If no parameters are passed, default to '.'
        parameters = data.get('parameters',['.'])

        # Ensure parameters is a list (this handles invalid cases)
        if not isinstance(parameters, list):
            return jsonify({
                'error': 'Invalid parameters',
                'message': 'The "parameters" field should be a list.'
            }), 400

    command = ['ls'] + parameters
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    output = {'STDOUT': result.stdout, 'STDERR': result.stderr}

    return Response(status=200,
                    content_type='application/json',
                    response=json.dumps(output)
                    )

@app.post('/download')
def download():
    if request.is_json:
        data = request.get_json()

        # Check if 'parameters' is provided in the JSON request
        if 'parameters' not in data:
            return jsonify({
                'error': 'Missing parameters',
                'message': 'Please provide the "parameters" field in the request body.'
            }), 400  # HTTP Status Code for Bad Request
        
        # If no parameters are passed, default to an empty string (meaning download from the root URL)
        parameters = data.get('parameters', [''])

        # Ensure parameters is a list (this handles invalid cases)
        if not isinstance(parameters, list):
            return jsonify({
                'error': 'Invalid parameters',
                'message': 'The "parameters" field should be a list.'
            }), 400

    command = ['wget'] + parameters
    print(command)
    result = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

    output = {'STDOUT': result.stdout, 'STDERR': result.stderr}

    return Response(status=200,
                    content_type='application/json',
                    response=json.dumps(output)
                    )

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=80, debug=True)
