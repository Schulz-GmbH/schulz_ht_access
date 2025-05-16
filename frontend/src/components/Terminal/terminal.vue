<template>
	<div class="flex flex-wrap flex-1 -mx-3 xl:flex-col" id="terminal">
		<div id="widget" class="w-full max-w-full px-3 mb-6 xl:w-full">
			<div
				class="relative h-full flex flex-col min-w-0 break-words bg-white shadow-soft-xl rounded-2xl bg-clip-border">
				<div class="h-full flex flex-col flex-auto p-4">
					<div class="flex flex-none flex-row flex-nowrap !hidden">
						<div class="flex-none px-3 w-1/2 xl:w-2/3">
							<p class="mb-0 font-sans font-semibold leading-normal text-sm"
								:class="systemStore.serial.available && systemStore.serial.connected ? 'text-lime-500' : 'text-red-500'">
								{{ systemStore.serial.available && systemStore.serial.connected ? 'Verbunden' :
									'Getrennt' }}
							</p>
						</div>
					</div>
					<div class="flex flex-1 flex-col flex-nowrap gap-6">
						<div class="flex-1">
							<div class="flex flex-col w-full h-full">
								<div class="terminal-content w-full flex flex-1 flex-row flex-nowrap gap-6">
									<div
										class="w-full h-full bg-gray-50 border border-solid border-gray-300 rounded-lg">
										<div class="terminal-scroll overflow-y-auto h-full">
											<textarea ref="terminalTextarea" v-model="output" name="terminalOutput"
												id="terminalOutput" readonly
												class="w-full h-full bg-transparent border-none outline-none no-scrollbar">
										</textarea>
										</div>
									</div>
								</div>
							</div>
						</div>
					</div>
				</div>
			</div>
		</div>
	</div>
</template>

<script lang="ts">
export { default } from './terminal'
</script>

<style src="./terminal.scss"></style>

<style lang="scss" scoped></style>
