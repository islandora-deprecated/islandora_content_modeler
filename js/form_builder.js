function hideAndClearElement(name) {
    $("#edit-"+name).val("");
    $("#edit-"+name+"-wrapper").hide();
}
function showElement(name) {
    $("#edit-"+name+"-wrapper").show();
}

function showNodeCreationType(value) {
    switch(value) {
        case 'Element':
            hideAndClearElement('attribute');
            hideAndClearElement('xml');
            showElement("element");
            break;
        case 'Attribute':
            hideAndClearElement('element');
            hideAndClearElement('xml');
            showElement("attribute");
            break;
        case 'XML':
            hideAndClearElement('attribute');
            hideAndClearElement('element');
            showElement("xml");
            break;
    }
}

$(document).ready(function() {
    $('#edit-full').live('change', function() {
        if($(this).attr('checked')) {
            $("#edit-parent-path-wrapper").show();
        }
        else {
            $("#edit-parent-path").val('');
            $("#edit-parent-path-wrapper").hide();
        }
    });
    $('#edit-select').live('change', function() {
        showNodeCreationType($(this).val());
    });
    $("#ajaxForm").ajaxComplete(function(event, request, settings) {
        if($('#edit-full').attr('checked') == false) {
            $("#edit-parent-path-wrapper").hide();
        }
        showNodeCreationType($('#edit-select').val());
    });
});